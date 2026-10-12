/*
 * SPDX-FileCopyrightText: 2026 Ngó Sen contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "ngosen-strings.h"

#include "ngosen-utf8.h"

#include <algorithm>
#include <string_view>
#include <unordered_map>

bool isStartsWith(const std::string& str, const std::string& prefix) {
#if __cplusplus >= 202002L
    return str.starts_with(prefix);
#else
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
#endif
}

std::string stripDesktopSuffix(const std::string& program) {
    static constexpr std::string_view suffix = ".desktop";
    if (program.size() > suffix.size() && program.compare(program.size() - suffix.size(), suffix.size(), suffix) == 0) {
        return program.substr(0, program.size() - suffix.size());
    }
    return program;
}

std::string withoutToneMark(const std::string& word) {
    // Each row is a vowel followed by its sắc, huyền, hỏi, ngã and nặng forms.
    static const char* const rows[] = {
        "aáàảãạ", "ăắằẳẵặ", "âấầẩẫậ", "eéèẻẽẹ", "êếềểễệ", "iíìỉĩị", "oóòỏõọ", "ôốồổỗộ", "ơớờởỡợ", "uúùủũụ", "ưứừửữự", "yýỳỷỹỵ",
        "AÁÀẢÃẠ", "ĂẮẰẲẴẶ", "ÂẤẦẨẪẬ", "EÉÈẺẼẸ", "ÊẾỀỂỄỆ", "IÍÌỈĨỊ", "OÓÒỎÕỌ", "ÔỐỒỔỖỘ", "ƠỚỜỞỠỢ", "UÚÙỦŨỤ", "ƯỨỪỬỮỰ", "YÝỲỶỸỴ",
    };
    static const auto toneless = [] {
        std::unordered_map<std::string, std::string> map;
        for (const std::string_view row : rows) {
            const auto base = std::string(row.substr(0, ngosen::utf8::charBytes(row)));
            for (size_t i = base.size(); i < row.size();) {
                const size_t bytes = ngosen::utf8::charBytes(row.substr(i));
                map.emplace(std::string(row.substr(i, bytes)), base);
                i += bytes;
            }
        }
        return map;
    }();

    std::string result;
    for (size_t i = 0; i < word.size();) {
        // A NUL byte counts as zero bytes; copy it like any other byte.
        const size_t bytes = std::max<size_t>(1, ngosen::utf8::charBytes(std::string_view(word).substr(i)));
        const auto   ch    = word.substr(i, bytes);
        const auto   found = toneless.find(ch);
        result += found == toneless.end() ? ch : found->second;
        i += bytes;
    }
    return result;
}
