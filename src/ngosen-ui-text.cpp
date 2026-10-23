// SPDX-License-Identifier: GPL-3.0-or-later
#include "ngosen-ui-text.h"

#include "ngosen-log.h"

#include <cstdint>
#include <fstream>
#include <iterator>
#include <optional>
#include <unordered_map>

namespace ngosen {

    namespace {

        bool                                                        vietnamese = true;
        std::optional<std::unordered_map<std::string, std::string>> catalog;

        std::optional<std::uint32_t>                                readWord(const std::string& data, std::size_t at, bool swapped) {
            if (at + 4 > data.size())
                return std::nullopt;
            std::uint32_t word = 0;
            for (int i = 0; i < 4; ++i) {
                const auto byte = static_cast<std::uint8_t>(data[at + (swapped ? 3 - i : i)]);
                word |= static_cast<std::uint32_t>(byte) << (8 * i);
            }
            return word;
        }

        std::optional<std::string> readString(const std::string& data, std::size_t table, std::uint32_t index, bool swapped) {
            const auto length = readWord(data, table + 8 * std::size_t{index}, swapped);
            const auto offset = readWord(data, table + 8 * std::size_t{index} + 4, swapped);
            if (!length || !offset || std::size_t{*offset} + *length > data.size())
                return std::nullopt;
            // Plural forms follow the first after a NUL; the menu has none.
            auto text = data.substr(*offset, *length);
            return text.substr(0, text.find('\0'));
        }

    } // namespace

    bool loadUiCatalog(const std::string& path) {
        std::ifstream     file(path, std::ios::binary);
        const std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        const auto        magic = readWord(data, 0, false);
        if (!magic || (*magic != 0x950412de && *magic != 0xde120495))
            return false;
        const bool swapped    = *magic == 0xde120495;
        const auto count      = readWord(data, 8, swapped);
        const auto originals  = readWord(data, 12, swapped);
        const auto translated = readWord(data, 16, swapped);
        if (!count || !originals || !translated)
            return false;
        std::unordered_map<std::string, std::string> strings;
        for (std::uint32_t i = 0; i < *count; ++i) {
            auto english     = readString(data, *originals, i, swapped);
            auto translation = readString(data, *translated, i, swapped);
            if (!english || !translation)
                return false;
            if (!english->empty() && !translation->empty())
                strings.emplace(std::move(*english), std::move(*translation));
        }
        catalog = std::move(strings);
        return true;
    }

    void setUiVietnamese(bool value) {
        vietnamese = value;
    }

    std::string uiText(const std::string& english) {
        if (!vietnamese)
            return english;
        if (!catalog) {
            const std::string path = NGOSEN_LOCALEDIR "/vi/LC_MESSAGES/fcitx5-ngosen.mo";
            if (!loadUiCatalog(path)) {
                NGOSEN_WARN("Could not read " + path + ", the menu stays English");
                catalog.emplace();
            }
        }
        const auto found = catalog->find(english);
        return found != catalog->end() ? found->second : english;
    }

} // namespace ngosen
