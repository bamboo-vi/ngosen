/*
 * SPDX-FileCopyrightText: 2026 Ngó Sen contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include <string>

// A Flatpak app reports its app ID (org.mozilla.firefox) where the distro package reports its
// program name (firefox); the app checks are written against the program name.
namespace ngosen {

    // True for a reverse-DNS name with at least three parts, the form Flatpak requires of app IDs.
    bool isFlatpakAppId(const std::string& name);

    // The lowercased StartupWMClass of a desktop file's [Desktop Entry] group, or "" without one.
    std::string startupWmClass(const std::string& desktopEntry);

    // The program name for a Flatpak app ID: the desktop file's StartupWMClass, else the ID's last part.
    std::string programForAppId(const std::string& appId, const std::string& desktopEntry);

    // The reported program name with a Flatpak app ID replaced by its program name; reads the
    // exported desktop file once per ID.
    std::string normalizeProgramName(const std::string& program);

} // namespace ngosen
