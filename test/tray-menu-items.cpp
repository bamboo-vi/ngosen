// SPDX-License-Identifier: GPL-3.0-or-later
//
// The tray menu shows the entries the user chose; Settings and Save typing log always show.
#include "ngosen-engine.h"
#include "test-input-context.h"

#include <fcitx/action.h>
#include <fcitx/statusarea.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace {

    int  failures = 0;

    void check(const std::string& step, bool ok) {
        if (!ok) {
            std::cerr << "FAIL: " << step << '\n';
            ++failures;
        }
    }

    std::vector<std::string> menu(TestInputContext& context) {
        std::vector<std::string> names;
        for (auto* action : context.statusArea().actions(fcitx::StatusGroup::InputMethod))
            names.push_back(action->name());
        return names;
    }

    bool has(const std::vector<std::string>& names, const std::string& name) {
        return std::find(names.begin(), names.end(), name) != names.end();
    }

} // namespace

int main() {
    configureTestPaths("fcitx5-ngosen-tray-menu-items");
    TestInstance        testInstance;
    fcitx::NgoSenEngine engine(&testInstance.instance);

    TestInputContext    context(&testInstance.instance);
    context.focusIn();
    fcitx::InputMethodEntry  entry("ngosen", "Ngó Sen", "vi", "ngosen");
    fcitx::InputContextEvent in(&context, fcitx::EventType::InputContextFocusIn);
    engine.activate(entry, in);

    auto names = menu(context);
    check("every entry shows by default", names.size() == 10 && has(names, "ngosen-input-method") && has(names, "ngosen-charset"));

    fcitx::RawConfig hidden;
    hidden.setValueByPath("ShowMenuCharset", "False");
    engine.setConfig(hidden);
    check("the focused field's menu changes at once", !has(menu(context), "ngosen-charset"));
    engine.activate(entry, in);
    names = menu(context);
    check("a hidden entry leaves the menu", !has(names, "ngosen-charset") && names.size() == 9);
    check("Settings and Save typing log stay", has(names, "ngosen-settings") && has(names, "ngosen-save-log"));

    // The tray keeps showing the last field's menu while the settings window has focus.
    context.focusOut();
    hidden.setValueByPath("ShowMenuSpellCheck", "False");
    engine.setConfig(hidden);
    check("the menu left in the tray changes at once", !has(menu(context), "ngosen-spellcheck"));
    context.focusIn();
    engine.activate(entry, in);

    const auto before = menu(context).size();
    engine.activate(entry, in);
    check("filling again does not repeat entries", menu(context).size() == before);

    return failures == 0 ? 0 : 1;
}
