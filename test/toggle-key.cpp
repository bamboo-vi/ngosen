// SPDX-License-Identifier: GPL-3.0-or-later
//
// The toggle key switches between English and Vietnamese for every app without a rule of its own,
// and English lets keys through untouched.
#include "ngosen-engine.h"
#include "ngosen-utils.h"
#include "test-input-context.h"

#include <fcitx/candidatelist.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

namespace {

    int  failures = 0;

    void check(bool ok, const std::string& what, const std::string& actual) {
        if (!ok) {
            std::cerr << "FAIL: " << what << "\n  got: " << actual << '\n';
            ++failures;
        }
    }

    std::string modeName(ngosen::Mode mode) {
        switch (mode) {
            case ngosen::Mode::Off: return "Off";
            case ngosen::Mode::Sen: return "Sen";
            case ngosen::Mode::Preedit: return "Preedit";
            case ngosen::Mode::Emoji: return "Emoji";
        }
        return "?";
    }

    struct Keys {
        fcitx::NgoSenEngine&           engine;
        const fcitx::InputMethodEntry& entry;

        // Returns whether the engine kept the key from the app.
        bool send(TestInputContext& context, fcitx::Key key, bool release) {
            fcitx::KeyEvent event(&context, key, release);
            engine.keyEvent(entry, event);
            return event.filtered();
        }

        bool tap(TestInputContext& context, const char* key) {
            const bool kept = send(context, fcitx::Key(key), false);
            send(context, fcitx::Key(key), true);
            return kept;
        }

        // Ctrl held, Shift pressed and released, then Ctrl released, with `between` typed while both are down.
        void ctrlShift(TestInputContext& context, fcitx::KeySym between = FcitxKey_None) {
            const fcitx::KeyStates ctrl{fcitx::KeyState::Ctrl};
            const fcitx::KeyStates both{fcitx::KeyState::Ctrl, fcitx::KeyState::Shift};
            send(context, fcitx::Key(FcitxKey_Control_L), false);
            send(context, fcitx::Key(FcitxKey_Shift_L, ctrl), false);
            if (between != FcitxKey_None) {
                send(context, fcitx::Key(between, both), false);
                send(context, fcitx::Key(between, both), true);
            }
            send(context, fcitx::Key(FcitxKey_Shift_L, both), true);
            send(context, fcitx::Key(FcitxKey_Control_L, ctrl), true);
        }

        void focus(TestInputContext& context) {
            context.focusIn();
            fcitx::InputContextEvent event(&context, fcitx::EventType::InputContextFocusIn);
            engine.activate(entry, event);
        }
    };

    std::string savedConfig() {
        std::ifstream     file(std::string(getenv("XDG_CONFIG_HOME")) + "/fcitx5/conf/ngosen.conf");
        std::stringstream text;
        text << file.rdbuf();
        return text.str();
    }

    bool menuOpen(TestInputContext& context) {
        auto list = context.inputPanel().candidateList();
        return list && list->size() > 1;
    }

} // namespace

int main() {
    configureTestPaths("fcitx5-ngosen-toggle-key");
    TestInstance            testInstance;
    fcitx::NgoSenEngine     engine(&testInstance.instance);
    fcitx::InputMethodEntry entry("ngosen", "Ngó Sen", "vi", "ngosen");
    Keys                    keys{engine, entry};

    auto                    gedit = std::make_unique<TestInputContext>(&testInstance.instance, "gedit");
    auto                    kate  = std::make_unique<TestInputContext>(&testInstance.instance, "kate");
    keys.focus(*gedit);
    check(realMode.load() == ngosen::Mode::Off, "a new install starts in English", modeName(realMode.load()));

    // English is as if no input method were there.
    check(!keys.tap(*gedit, "a"), "English lets letters through", "kept");
    check(!keys.tap(*gedit, "Control+c"), "English lets Ctrl+C through", "kept");
    check(!keys.tap(*gedit, "Control+space"), "Ctrl+Space is not the key on a new install", "kept");
    check(gedit->commits().empty(), "English commits nothing", std::to_string(gedit->commits().size()) + " commits");
    check(!keys.tap(*gedit, "grave"), "English lets ` through", "kept");
    check(!menuOpen(*gedit), "` opens no menu in English", "menu open");

    keys.ctrlShift(*gedit, FcitxKey_T);
    check(realMode.load() == ngosen::Mode::Off, "Ctrl+Shift+T does not switch", modeName(realMode.load()));
    keys.ctrlShift(*gedit);
    check(realMode.load() == ngosen::Mode::Sen, "Ctrl+Shift switches to Vietnamese", modeName(realMode.load()));
    check(savedConfig().find("VietnameseOn=True") != std::string::npos, "the choice is saved", savedConfig());

    // fcitx5 leaves the default toggle key out of the file the engine saved.
    engine.reloadConfig();
    keys.focus(*gedit);
    check(!keys.tap(*gedit, "Control+space"), "Ctrl+Space stays unused after a reload", "kept");
    const auto beforeReloadSwitch = realMode.load();
    keys.ctrlShift(*gedit);
    check(realMode.load() != beforeReloadSwitch, "Ctrl+Shift still switches after a reload", modeName(realMode.load()));
    keys.ctrlShift(*gedit);

    // No app has a rule, so the switch holds everywhere.
    keys.focus(*kate);
    check(realMode.load() == ngosen::Mode::Sen, "another app follows the switch", modeName(realMode.load()));
    keys.ctrlShift(*kate);
    keys.focus(*gedit);
    check(realMode.load() == ngosen::Mode::Off, "switching back holds in every app", modeName(realMode.load()));

    // gedit gets a rule of its own from the menu: Off.
    keys.ctrlShift(*gedit);
    keys.tap(*gedit, "grave");
    check(menuOpen(*gedit), "` opens the menu in Vietnamese", "no menu");
    keys.tap(*gedit, "e");
    check(realMode.load() == ngosen::Mode::Off, "the menu sets Off for gedit", modeName(realMode.load()));
    keys.ctrlShift(*gedit);
    check(realMode.load() == ngosen::Mode::Sen, "the toggle switches gedit's own rule", modeName(realMode.load()));
    keys.focus(*kate);
    keys.ctrlShift(*kate);
    check(realMode.load() == ngosen::Mode::Off, "kate switches to English", modeName(realMode.load()));
    keys.focus(*gedit);
    check(realMode.load() == ngosen::Mode::Sen, "gedit keeps its rule", modeName(realMode.load()));

    // Settings saved before the toggle key existed keep the key fcitx5 used to switch with.
    {
        std::ofstream file(std::string(getenv("XDG_CONFIG_HOME")) + "/fcitx5/conf/ngosen.conf");
        file << "Mode=Sen\nInputMethod=Telex\n";
    }
    engine.reloadConfig();
    keys.focus(*kate);
    check(realMode.load() == ngosen::Mode::Sen, "older settings start in Vietnamese", modeName(realMode.load()));
    const auto before = realMode.load();
    check(keys.tap(*kate, "Control+space"), "older settings switch with Ctrl+Space", "passed to the app");
    check(realMode.load() != before, "Ctrl+Space switches with older settings", modeName(realMode.load()));
    keys.ctrlShift(*kate);
    check(realMode.load() != before, "Ctrl+Shift does not switch with older settings", modeName(realMode.load()));

    {
        std::ofstream file(std::string(getenv("XDG_CONFIG_HOME")) + "/fcitx5/conf/ngosen.conf");
        file << "Mode=OFF\n";
    }
    engine.reloadConfig();
    auto mail = std::make_unique<TestInputContext>(&testInstance.instance, "mail");
    keys.focus(*mail);
    check(realMode.load() == ngosen::Mode::Off, "older settings with OFF as the mode start in English", modeName(realMode.load()));

    // The word being typed is finished in the mode it was typed in. A Ctrl press commits the preedit
    // anyway, so the key here is one without modifiers.
    fcitx::RawConfig preedit;
    preedit.setValueByPath("Mode", "Preedit");
    preedit.setValueByPath("VietnameseOn", "True");
    preedit.setValueByPath("ToggleKey/0", "F12");
    preedit.setValueByPath("ToggleKey/1", "Control+Shift_L");
    engine.setConfig(preedit);
    auto notes = std::make_unique<TestInputContext>(&testInstance.instance, "notes");
    notes->setCapabilityFlags(fcitx::CapabilityFlag::Preedit);
    keys.focus(*notes);
    for (const char* key : {"v", "i", "e", "e", "t", "j"})
        keys.tap(*notes, key);
    keys.tap(*notes, "F12");
    const std::string committed = notes->commits().empty() ? "(nothing)" : notes->commits().back();
    check(committed == "việt", "switching to English commits the word in preedit", committed);

#if NGOSEN_USE_MODERN_FCITX_API
    // Held past fcitx5's limit for modifier-only keys, as around a Ctrl+Shift+click.
    fcitx::RawConfig limit;
    limit.setValueByPath("Hotkey/ModifierOnlyKeyTimeout", "100");
    testInstance.instance.globalConfig().load(limit, true);
    keys.send(*notes, fcitx::Key(FcitxKey_Control_L), false);
    keys.send(*notes, fcitx::Key(FcitxKey_Shift_L, fcitx::KeyStates{fcitx::KeyState::Ctrl}), false);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    // A held key repeats, and the repeat must not restart the hold.
    keys.send(*notes, fcitx::Key(FcitxKey_Shift_L, fcitx::KeyStates{fcitx::KeyState::Ctrl}), false);
    keys.send(*notes, fcitx::Key(FcitxKey_Shift_L, fcitx::KeyStates{fcitx::KeyState::Ctrl, fcitx::KeyState::Shift}), true);
    check(realMode.load() == ngosen::Mode::Off, "a long Ctrl+Shift hold does not switch", modeName(realMode.load()));
#endif

    return failures == 0 ? 0 : 1;
}
