# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
"""Recording a key in the hotkey field, as fcitx5 names it."""

import unittest

from qtpy.QtCore import QEvent, Qt
from qtpy.QtGui import QKeyEvent
from support import app
from ui.components import HotkeyCaptureWidget

KEYSYMS = {"Control_L": 0xFFE3, "Shift_L": 0xFFE1, "Shift_R": 0xFFE2, "a": 0x61}
QT_KEYS = {
    "Control_L": Qt.Key_Control,
    "Shift_L": Qt.Key_Shift,
    "Shift_R": Qt.Key_Shift,
    "a": Qt.Key_A,
}


class HotkeyCaptureTest(unittest.TestCase):
    def setUp(self):
        self.widget = HotkeyCaptureWidget()
        self.recorded = []
        self.widget.textChanged.connect(self.recorded.append)
        self.widget.setChecked(True)

    def key(self, name, release=False, modifiers=Qt.NoModifier):
        kind = QEvent.KeyRelease if release else QEvent.KeyPress
        event = QKeyEvent(
            kind, QT_KEYS[name], modifiers, 0, KEYSYMS[name], 0, "a" if name == "a" else ""
        )
        app.sendEvent(self.widget, event)

    def test_ctrl_shift_alone_is_recorded(self):
        self.key("Control_L")
        self.key("Shift_L", modifiers=Qt.ControlModifier)
        self.key("Shift_L", release=True, modifiers=Qt.ControlModifier | Qt.ShiftModifier)
        self.assertEqual(self.recorded, ["Control+Shift_L"])
        self.assertFalse(self.widget.isChecked())

    def test_one_modifier_alone_is_recorded(self):
        self.key("Shift_R")
        self.key("Shift_R", release=True, modifiers=Qt.ShiftModifier)
        self.assertEqual(self.recorded, ["Shift_R"])

    def test_a_key_after_modifiers_is_recorded_with_them(self):
        self.key("Control_L")
        self.key("a", modifiers=Qt.ControlModifier)
        self.key("Control_L", release=True, modifiers=Qt.ControlModifier)
        self.assertEqual(self.recorded, ["Control+A"])


if __name__ == "__main__":
    unittest.main()
