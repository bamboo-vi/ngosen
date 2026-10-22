# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
import unittest

from qtpy.QtCore import QEvent
from qtpy.QtWidgets import QCheckBox, QFormLayout, QLabel, QLineEdit, QListWidget
from support import FakeDBusHandler, app
from ui.components import SingleKeyCaptureWidget
from ui.main_window import NgoSenSettingsWindow
from ui.pages.dynamic_settings import DynamicSettingsPage, SettingsCategory


def form_of(page):
    app.sendPostedEvents(None, QEvent.DeferredDelete)
    return page.findChild(QFormLayout)


def row_labels(form):
    labels = []
    for row in range(form.rowCount()):
        item = form.itemAt(row, QFormLayout.LabelRole)
        if item and isinstance(item.widget(), QLabel) and item.widget().text():
            labels.append(item.widget().text())
    return labels


class TypingPageTest(unittest.TestCase):
    def setUp(self):
        self.page = DynamicSettingsPage(FakeDBusHandler(), category=SettingsCategory.TYPING)

    def test_common_settings_come_first(self):
        self.assertEqual(
            row_labels(form_of(self.page))[:3], ["Mode:", "Input Method:", "Output Charset:"]
        )

    def test_options_are_grouped(self):
        labels = row_labels(form_of(self.page))
        self.assertIn("Tone marks:", labels)
        self.assertIn("Quick typing:", labels)

    def test_hint_sits_under_its_setting(self):
        cb = next(
            c for c in self.page.findChildren(QCheckBox) if c.text() == "Auto Restore Invalid Words"
        )
        hints = [w.text() for w in cb.parentWidget().findChildren(QLabel)]
        self.assertIn("Typing windows still gives windows.", hints)

    def test_hints_take_one_line(self):
        self.page.resize(760, 900)
        self.page.show()
        app.processEvents()
        hints = [
            label
            for label in self.page.findChildren(QLabel)
            if label.text()
            in (
                "Typing windows still gives windows.",
                "Words that are not Vietnamese get no tone marks.",
            )
        ]
        self.assertEqual(len(hints), 2)
        for hint in hints:
            self.assertLess(hint.height(), 2 * hint.fontMetrics().lineSpacing(), hint.text())

    def test_long_hint_wraps_instead_of_scrolling(self):
        self.page.resize(760, 900)
        self.page.show()
        app.processEvents()
        hint = next(
            label
            for label in self.page.findChildren(QLabel)
            if label.text().startswith("z in Telex (0 in VNI)")
        )
        self.assertGreaterEqual(hint.height(), 2 * hint.fontMetrics().lineSpacing())
        # The page may still scroll for other reasons; the hint must not be what forces it.
        one_line = hint.fontMetrics().boundingRect(hint.text()).width()
        self.assertLess(hint.minimumSizeHint().width(), one_line)

    def test_has_a_field_to_try_typing(self):
        form = form_of(self.page)
        last = form.itemAt(form.rowCount() - 1, QFormLayout.FieldRole).widget()
        self.assertIsInstance(last, QLineEdit)

    def test_labels_go_above_fields_in_a_narrow_window(self):
        self.page.resize(500, 600)
        self.page.show()
        app.processEvents()
        self.assertEqual(form_of(self.page).rowWrapPolicy(), QFormLayout.WrapAllRows)
        self.page.resize(900, 600)
        app.processEvents()
        self.assertEqual(form_of(self.page).rowWrapPolicy(), QFormLayout.DontWrapRows)

    def test_labels_go_above_fields_when_larger_text_does_not_fit(self):
        font = self.page.font()
        font.setPointSizeF(font.pointSizeF() * 1.4)
        self.page.setFont(font)
        self.page.resize(760, 600)
        self.page.show()
        app.processEvents()
        self.assertFalse(self.page.scroll.horizontalScrollBar().isVisible())


class ShortcutsPageTest(unittest.TestCase):
    def test_mode_keys_fit_in_the_list(self):
        page = DynamicSettingsPage(FakeDBusHandler(), category=SettingsCategory.SHORTCUTS)
        page.resize(800, 600)
        page.show()
        for _ in range(3):
            app.processEvents()
        view = page.findChild(QListWidget).viewport()
        for button in page.findChildren(SingleKeyCaptureWidget):
            self.assertLessEqual(button.mapTo(view, button.rect().bottomRight()).x(), view.width())


class SidebarTest(unittest.TestCase):
    def test_general_is_merged_into_typing(self):
        window = NgoSenSettingsWindow(dbus_handler=FakeDBusHandler())
        titles = [window.sidebar.item(r).text() for r in range(window.sidebar.count())]
        self.assertNotIn("General", titles)
        self.assertEqual(titles[0], "Typing")


if __name__ == "__main__":
    unittest.main()
