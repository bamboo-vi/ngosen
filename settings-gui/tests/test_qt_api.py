# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
import unittest

import qt_api


def installed(*modules):
    return lambda name: name in modules


class QtApiTest(unittest.TestCase):
    def test_qt6_wins_over_an_installed_pyqt5(self):
        env = {}
        qt_api.pick(env, installed("PyQt5", "PyQt6"))
        self.assertEqual(env, {"QT_API": "pyqt6"})

    def test_pyside6_when_there_is_no_pyqt6(self):
        env = {}
        qt_api.pick(env, installed("PyQt5", "PySide6"))
        self.assertEqual(env, {"QT_API": "pyside6"})

    def test_a_chosen_binding_is_kept(self):
        env = {"QT_API": "pyqt5"}
        qt_api.pick(env, installed("PyQt5", "PyQt6"))
        self.assertEqual(env, {"QT_API": "pyqt5"})

    def test_qt5_only_is_left_to_qtpy(self):
        env = {}
        qt_api.pick(env, installed("PyQt5"))
        self.assertEqual(env, {})


if __name__ == "__main__":
    unittest.main()
