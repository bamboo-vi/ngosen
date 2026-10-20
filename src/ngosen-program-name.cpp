/*
 * SPDX-FileCopyrightText: 2026 Ngó Sen contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "ngosen-program-name.h"

#include "ngosen-strings.h"
#include "ngosen-utils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace ngosen {

    namespace {
        std::string lowercase(std::string text) {
            std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return text;
        }

        std::string readExportedDesktopFile(const std::string& appId) {
            const std::string home = getEnv("HOME");
            for (const std::string& dir : {home + "/.local/share/flatpak", std::string("/var/lib/flatpak")}) {
                std::ifstream file(dir + "/exports/share/applications/" + appId + ".desktop");
                if (file) {
                    std::ostringstream text;
                    text << file.rdbuf();
                    return text.str();
                }
            }
            return {};
        }
    } // namespace

    bool isFlatpakAppId(const std::string& name) {
        size_t parts = 0;
        size_t start = 0;
        while (start <= name.size()) {
            const size_t end  = std::min(name.find('.', start), name.size());
            const auto   part = name.substr(start, end - start);
            if (part.empty() || std::isdigit(static_cast<unsigned char>(part[0])) ||
                !std::all_of(part.begin(), part.end(), [](unsigned char c) { return std::isalnum(c) || c == '_' || c == '-'; }))
                return false;
            ++parts;
            start = end + 1;
        }
        return parts >= 3;
    }

    std::string startupWmClass(const std::string& desktopEntry) {
        std::istringstream lines(desktopEntry);
        std::string        line;
        bool               inEntry = false;
        while (std::getline(lines, line)) {
            if (!line.empty() && line[0] == '[')
                inEntry = line == "[Desktop Entry]";
            else if (inEntry && isStartsWith(line, "StartupWMClass=")) {
                // Mullvad Browser's class is "Mullvad Browser"; its program is mullvadbrowser.
                std::string wmClass = line.substr(std::string("StartupWMClass=").size());
                wmClass.erase(std::remove(wmClass.begin(), wmClass.end(), ' '), wmClass.end());
                return lowercase(wmClass);
            }
        }
        return {};
    }

    std::string programForAppId(const std::string& appId, const std::string& desktopEntry) {
        std::string program = startupWmClass(desktopEntry);
        // Some apps (Lutris) give their app ID as the class too.
        if (program.empty() || isFlatpakAppId(program))
            program = lowercase(appId.substr(appId.rfind('.') + 1));
        return program;
    }

    std::string normalizeProgramName(const std::string& program) {
        // The IBus GTK4 module puts this before the name it reports.
        static const std::string gtk4Prefix = "gtk4-im:";
        if (isStartsWith(program, gtk4Prefix))
            return gtk4Prefix + normalizeProgramName(program.substr(gtk4Prefix.size()));

        const std::string name = stripDesktopSuffix(program);
        if (!isFlatpakAppId(name))
            return program;
        static std::unordered_map<std::string, std::string> cache;
        auto                                                it = cache.find(name);
        if (it == cache.end())
            it = cache.emplace(name, programForAppId(name, readExportedDesktopFile(name))).first;
        return it->second;
    }

} // namespace ngosen
