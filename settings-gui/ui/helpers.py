# SPDX-FileCopyrightText: 2026 Nguyen Hoang Ky <nhktmdzhg@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
"""
Helper utilities and shared mappings for the Ngó Sen settings window.
"""

from i18n import N_, _

from ui.components import HelpIcon

# Tooltip text for specific settings keys
HELPERS = {
    "CapitalizeMacro": N_(
        "Automatically match expansion case to trigger key case.\n\n"
        "Example if 'kg' is 'Khô gà':\n"
        "- kg -> khô gà\n"
        "- Kg -> Khô gà\n"
        "- KG -> KHÔ GÀ"
    ),
    "EnableMacroInOffMode": N_(
        "Allow macros to work when the input mode is OFF.\n"
        "When disabled, macros are only available in active typing modes."
    ),
    "MacroSkipTriggerModifier": N_(
        "Press and release the selected modifier key before typing to skip macro expansion for the next word.\n"
        "If another key is pressed between the modifier's press and release, the skip is cancelled."
    ),
}


def add_help_icon(layout, key):
    """Adds a HelpIcon for a setting key that has help text."""
    # Only add if we have a mapped helper text
    helper_text = HELPERS.get(key)
    if helper_text:
        icon = HelpIcon(_(helper_text))
        layout.addWidget(icon)
        return icon
    return None
