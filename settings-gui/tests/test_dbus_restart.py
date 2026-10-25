# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
import importlib.util
import os
import shutil
import subprocess
import sys
import tempfile
import time
import unittest

# A stand-in for fcitx5's controller that writes each SetConfig to the file it is given.
FAKE_FCITX5 = """
import sys
import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

DBusGMainLoop(set_as_default=True)


class Controller(dbus.service.Object):
    @dbus.service.method("org.fcitx.Fcitx.Controller1", in_signature="s", out_signature="va(sa(sssva{sv}))")
    def GetConfig(self, uri):
        return dbus.Dictionary({"SpellCheck": "True"}, signature="sv", variant_level=1), []

    @dbus.service.method(
        "org.fcitx.Fcitx.Controller1", in_signature="sv", message_keyword="message"
    )
    def SetConfig(self, uri, values, message):
        # fcitx5 rejects a call whose wire signature differs from "sv"; dbus-python accepts it.
        if message.get_signature() != "sv":
            raise dbus.exceptions.DBusException(
                "got " + message.get_signature(), name="org.freedesktop.DBus.Error.InvalidArgs"
            )
        with open(sys.argv[1], "a") as f:
            f.write(" ".join(sorted(values)) + "\\n")


bus = dbus.SessionBus()
name = dbus.service.BusName("org.fcitx.Fcitx5", bus)
Controller(bus, "/controller")
GLib.MainLoop().run()
"""


@unittest.skipUnless(
    shutil.which("dbus-daemon") and importlib.util.find_spec("gi"), "needs dbus-daemon and gi"
)
class Fcitx5RestartTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.daemon = subprocess.Popen(
            ["dbus-daemon", "--session", "--nofork", "--print-address=1"],
            stdout=subprocess.PIPE,
            text=True,
        )
        self.env = dict(os.environ, DBUS_SESSION_BUS_ADDRESS=self.daemon.stdout.readline().strip())
        self.fcitx5 = None

    def tearDown(self):
        if self.fcitx5:
            self.fcitx5.kill()
            self.fcitx5.wait()
        self.daemon.kill()
        self.daemon.wait()
        self.tmp.cleanup()

    def start_fcitx5(self, log):
        if self.fcitx5:
            self.fcitx5.kill()
            self.fcitx5.wait()
        self.fcitx5 = subprocess.Popen([sys.executable, "-c", FAKE_FCITX5, log], env=self.env)
        probe = ["dbus-send", "--session", "--print-reply", "--dest=org.freedesktop.DBus", "/"]
        probe += ["org.freedesktop.DBus.GetNameOwner", "string:org.fcitx.Fcitx5"]
        for _ in range(100):
            if subprocess.run(probe, env=self.env, capture_output=True).returncode == 0:
                return
            time.sleep(0.05)
        self.fail("fake fcitx5 did not start")

    def test_saves_to_fcitx5_started_after_the_window(self):
        first = os.path.join(self.tmp.name, "first")
        second = os.path.join(self.tmp.name, "second")
        self.start_fcitx5(first)
        script = (
            "import sys; sys.path.insert(0, sys.argv[1]);"
            "from core.dbus_handler import NgoSenDBusHandler;"
            "h = NgoSenDBusHandler(); print(bool(h.get_config()), flush=True);"
            "sys.stdin.readline();"
            "print(h.set_config({'SpellCheck': 'False'}), flush=True);"
            "print(h.set_sub_config_list('ngosen-macro', 'Macro', [{'Key': 'vn'}]), flush=True)"
        )
        gui = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        window = subprocess.Popen(
            [sys.executable, "-c", script, gui],
            env=self.env,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            text=True,
        )
        self.assertEqual(window.stdout.readline().strip(), "True")
        self.start_fcitx5(second)
        out, _ = window.communicate("go\n", timeout=10)
        self.assertEqual(out.split(), ["True", "True"])
        with open(second) as f:
            self.assertEqual(f.read(), "SpellCheck\nMacro\n")


if __name__ == "__main__":
    unittest.main()
