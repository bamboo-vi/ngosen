// SPDX-License-Identifier: GPL-3.0-or-later
//
// Ngó Sen's own strings in the language picked in its settings. fcitx5 translates by the language
// it runs in, which on an English desktop would keep the menu English.
#pragma once

#include <string>

namespace ngosen {

    void        setUiVietnamese(bool vietnamese);
    std::string uiText(const std::string& english);
    // Reads the Vietnamese strings from a compiled gettext catalog; false when it cannot.
    bool loadUiCatalog(const std::string& path);

} // namespace ngosen
