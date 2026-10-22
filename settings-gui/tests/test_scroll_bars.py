# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
import unittest

from qtpy.QtGui import QColor, QPalette
from qtpy.QtWidgets import QLabel, QScrollArea
from support import app
from ui import scroll_bars


def scroll_area():
    area = QScrollArea()
    area.setWidget(QLabel("\n".join(str(i) for i in range(100))))
    area.resize(200, 200)
    area.show()
    app.processEvents()
    return area


class ScrollBarsTest(unittest.TestCase):
    def setUp(self):
        self.style = app.style().name()
        app.setStyle("Fusion")

    def tearDown(self):
        app.setStyle(self.style)

    def install(self, desktop):
        styler = scroll_bars.install(app, {"XDG_CURRENT_DESKTOP": desktop})
        if styler:
            self.addCleanup(app.removeEventFilter, styler)
        return styler

    def test_gnome_gets_thin_bars(self):
        self.assertIsNotNone(self.install("ubuntu:GNOME"))
        self.area = scroll_area()
        bar = self.area.verticalScrollBar()
        self.assertEqual(bar.styleSheet(), scroll_bars.style_sheet(app.palette()))
        self.assertLessEqual(bar.width(), 10)

    def test_bars_follow_a_switch_to_dark(self):
        self.install("GNOME")
        self.area = scroll_area()
        bar = self.area.verticalScrollBar()
        light = app.palette()
        self.addCleanup(app.setPalette, light)
        dark = QPalette(light)
        dark.setColor(QPalette.WindowText, QColor("#ffffff"))
        app.setPalette(dark)
        app.processEvents()
        self.assertIn("rgba(255, 255, 255, 0.35)", bar.styleSheet())

    def test_other_desktops_keep_their_bars(self):
        self.assertIsNone(self.install("KDE"))
        self.area = scroll_area()
        bar = self.area.verticalScrollBar()
        self.assertEqual(bar.styleSheet(), "")
        self.assertGreater(bar.width(), 10)

    def test_an_installed_qt_theme_is_left_alone(self):
        app.setStyle("Windows")
        self.assertIsNone(self.install("GNOME"))


if __name__ == "__main__":
    unittest.main()
