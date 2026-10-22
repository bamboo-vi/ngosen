# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
import os
import tempfile
import time
import unittest

from support import app
from ui import color_scheme

# Prints the scheme for "get"; for "monitor", switches to light a moment in.
FAKE_GSETTINGS = """#!/bin/sh
if [ "$1" = get ]; then echo "'prefer-dark'"; exit; fi
sleep 0.3
echo "color-scheme: 'default'"
sleep 30
"""


class FakeHints:
    def __init__(self):
        self.asked = []

    def setColorScheme(self, scheme):
        self.asked.append(scheme.name)


@unittest.skipUnless(hasattr(app.styleHints(), "setColorScheme"), "Qt before 6.8")
class ColorSchemeTest(unittest.TestCase):
    def setUp(self):
        folder = tempfile.mkdtemp()
        self.gsettings = os.path.join(folder, "gsettings")
        with open(self.gsettings, "w") as f:
            f.write(FAKE_GSETTINGS)
        os.chmod(self.gsettings, 0o755)

    def test_follows_gnome_from_start_and_on_change(self):
        hints = FakeHints()
        follower = color_scheme.Follower(app, hints, self.gsettings)
        self.addCleanup(follower.stop)
        self.assertEqual(hints.asked, ["Dark"])
        end = time.time() + 3
        while len(hints.asked) < 2 and time.time() < end:
            app.processEvents()
            time.sleep(0.02)
        self.assertEqual(hints.asked, ["Dark", "Light"])

    def test_other_desktops_are_left_to_qt(self):
        self.assertIsNone(color_scheme.install(app, {"XDG_CURRENT_DESKTOP": "KDE"}))


if __name__ == "__main__":
    unittest.main()
