# SPDX-FileCopyrightText: 2026 Ngó Sen contributors
#
# SPDX-License-Identifier: GPL-3.0-or-later
"""Thin, rounded scroll bars on GNOME, where Qt has no GNOME style and falls back to Fusion's."""

import os

from qtpy.QtCore import QEvent, QObject
from qtpy.QtGui import QPalette
from qtpy.QtWidgets import QScrollBar

STYLE = """
QScrollBar:vertical {{ background: transparent; width: 10px; margin: 2px 2px 2px 0; }}
QScrollBar:horizontal {{ background: transparent; height: 10px; margin: 0 2px 2px 2px; }}
QScrollBar::handle {{ background: {handle}; border-radius: 4px; }}
QScrollBar::handle:vertical {{ min-height: 40px; }}
QScrollBar::handle:horizontal {{ min-width: 40px; }}
QScrollBar::handle:hover, QScrollBar::handle:pressed {{ background: {hover}; }}
QScrollBar::add-line, QScrollBar::sub-line {{ width: 0; height: 0; }}
QScrollBar::add-page, QScrollBar::sub-page {{ background: none; }}
"""


def style_sheet(palette):
    # Like GNOME, the text colour seen through: the palette's Mid is near white there.
    text = palette.color(QPalette.WindowText)
    rgba = "rgba({}, {}, {}, {{}})".format(text.red(), text.green(), text.blue())
    return STYLE.format(handle=rgba.format(0.35), hover=rgba.format(0.55))


class _Styler(QObject):
    def __init__(self, app):
        super().__init__(app)
        self._app = app
        self._sheet = style_sheet(app.palette())

    def eventFilter(self, obj, event):
        if event.type() == QEvent.Polish and isinstance(obj, QScrollBar) and not obj.styleSheet():
            obj.setStyleSheet(self._sheet)
        elif event.type() == QEvent.ApplicationPaletteChange and obj is self._app:
            self._sheet = style_sheet(self._app.palette())
            for widget in self._app.allWidgets():
                if isinstance(widget, QScrollBar) and widget.styleSheet():
                    widget.setStyleSheet(self._sheet)
        return False


def install(app, environ=os.environ):
    """Styles every scroll bar the app creates; returns the filter, or None when not needed.

    A Qt theme the user installed (Kvantum, adwaita-qt) replaces Fusion and is left alone.
    """
    desktops = environ.get("XDG_CURRENT_DESKTOP", "").upper().split(":")
    if "GNOME" not in desktops or app.style().name().lower() != "fusion":
        return None
    styler = _Styler(app)
    app.installEventFilter(styler)
    return styler
