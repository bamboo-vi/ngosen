# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
"""Picks the Qt binding before qtpy is imported."""

import importlib.util
import os

QT6_BINDINGS = (("pyqt6", "PyQt6"), ("pyside6", "PySide6"))


def pick(env=os.environ, installed=importlib.util.find_spec):
    """Prefers Qt 6, which the packages depend on."""
    # qtpy tries PyQt5 first, and a PyQt5 another package pulled in may lack the Wayland plugin.
    if env.get("QT_API"):
        return
    for api, module in QT6_BINDINGS:
        if installed(module):
            env["QT_API"] = api
            return


pick()
