# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
"""Follows GNOME's light or dark style.

On GNOME, Qt reads the style as light at startup and, on a switch to dark, changes how Fusion draws
but not the palette, so checkbox frames vanish on a light window. Asking Qt for the scheme makes it
switch the palette too.
"""

import os
import shutil

from qtpy.QtCore import QObject, QProcess, Qt

SCHEMA = ["org.gnome.desktop.interface", "color-scheme"]


def scheme_for(value):
    return Qt.ColorScheme.Dark if "prefer-dark" in value else Qt.ColorScheme.Light


def _text(process):
    return bytes(process.readAllStandardOutput()).decode(errors="replace")


class Follower(QObject):
    def __init__(self, app, hints, gsettings):
        super().__init__(app)
        self._hints = hints
        current = QProcess()
        current.start(gsettings, ["get", *SCHEMA])
        current.waitForFinished(2000)
        self._apply(_text(current))
        self._monitor = QProcess(self)
        self._monitor.readyReadStandardOutput.connect(self._changed)
        self._monitor.start(gsettings, ["monitor", *SCHEMA])
        app.aboutToQuit.connect(self.stop)

    def stop(self):
        self._monitor.kill()
        self._monitor.waitForFinished(1000)

    def _apply(self, value):
        self._hints.setColorScheme(scheme_for(value))

    def _changed(self):
        lines = _text(self._monitor).splitlines()
        if lines:
            self._apply(lines[-1])


def install(app, environ=os.environ):
    """Sets Qt's scheme from GNOME's now and on every change; returns the follower, or None."""
    desktops = environ.get("XDG_CURRENT_DESKTOP", "").upper().split(":")
    gsettings = shutil.which("gsettings")
    # setColorScheme is new in Qt 6.8; older Fusion draws the same either way.
    if "GNOME" not in desktops or not gsettings or not hasattr(app.styleHints(), "setColorScheme"):
        return None
    return Follower(app, app.styleHints(), gsettings)
