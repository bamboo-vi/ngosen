# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
import unittest

from qtpy.QtWidgets import QLabel, QListWidget
from support import FakeDBusHandler, app  # noqa: F401
from ui.pages.dynamic_settings import DynamicSettingsPage, SettingsCategory


class DuplicateShortcutTest(unittest.TestCase):
    def setUp(self):
        self.page = DynamicSettingsPage(FakeDBusHandler(), category=SettingsCategory.SHORTCUTS)
        self.page.resize(700, 900)
        self.page.show()
        app.processEvents()
        self.addCleanup(self.page.close)
        self.rows = self.page.findChild(QListWidget)

    def assert_rows_fit(self):
        total = 0
        for i in range(self.rows.count()):
            item = self.rows.item(i)
            needed = self.rows.itemWidget(item).sizeHint().height()
            self.assertGreaterEqual(item.sizeHint().height(), needed, f"row {i} is squashed")
            total += item.sizeHint().height()
        self.assertGreaterEqual(self.rows.height(), total, "the list cuts off its last rows")

    def warnings(self):
        return [w for w in self.page.findChildren(QLabel, "ShortcutWarning") if not w.isHidden()]

    def test_rows_grow_to_fit_the_warning(self):
        self.page.update_config("ShortcutOff", "1")
        app.processEvents()
        self.assertEqual(len(self.warnings()), 2)
        self.assert_rows_fit()

    def test_rows_shrink_back_when_fixed(self):
        self.page.update_config("ShortcutOff", "1")
        self.page.update_config("ShortcutOff", "e")
        app.processEvents()
        self.assertEqual(self.warnings(), [])
        self.assert_rows_fit()
        heights = {self.rows.item(i).sizeHint().height() for i in range(self.rows.count())}
        self.assertEqual(len(heights), 1, "every row is back to one line")


class RowWidthTest(unittest.TestCase):
    def setUp(self):
        self.page = DynamicSettingsPage(FakeDBusHandler(), category=SettingsCategory.SHORTCUTS)
        self.page.resize(600, 900)
        self.page.show()
        app.processEvents()
        self.addCleanup(self.page.close)
        self.rows = self.page.findChild(QListWidget)

    def resize_to(self, width):
        self.page.resize(width, 900)
        app.processEvents()

    def assert_rows_match_the_list(self):
        visible = self.rows.viewport().width()
        for i in range(self.rows.count()):
            self.assertEqual(self.rows.itemWidget(self.rows.item(i)).width(), visible, f"row {i}")

    def test_rows_widen_with_the_window(self):
        self.resize_to(1100)
        self.assert_rows_match_the_list()

    def test_rows_narrow_with_the_window(self):
        self.resize_to(1400)
        self.resize_to(700)
        self.assert_rows_match_the_list()


if __name__ == "__main__":
    unittest.main()
