// SPDX-License-Identifier: GPL-3.0-or-later
//
// Keys typed in English, in Ngó Sen or in another input method, are offered for retyping when
// Vietnamese comes on, unless focus left the field or a click moved the cursor in between.
#include "ngosen-engine.h"
#include "ngosen-utils.h"
#include "test-input-context.h"

#include <fcitx/candidatelist.h>

#include <iostream>
#include <memory>
#include <string>

namespace {

    int  failures = 0;

    void check(bool ok, const std::string& what, const std::string& actual) {
        if (!ok) {
            std::cerr << "FAIL: " << what << "\n  got: " << actual << '\n';
            ++failures;
        }
    }

    // Through the instance, as the app sends it, so the engine sees it only from its watcher.
    void typeWhileOff(TestInputContext& context, const std::string& text) {
        for (const char c : text) {
            fcitx::KeyEvent down(&context, fcitx::Key(static_cast<fcitx::KeySym>(c)), false);
            context.keyEvent(down);
            fcitx::KeyEvent up(&context, fcitx::Key(static_cast<fcitx::KeySym>(c)), true);
            context.keyEvent(up);
        }
    }

    // The key watcher sees the switch key like any other key.
    void pressSwitchKey(TestInputContext& context, const char* key = "Control+space") {
        fcitx::KeyEvent down(&context, fcitx::Key(key), false);
        context.keyEvent(down);
        fcitx::KeyEvent up(&context, fcitx::Key(key), true);
        context.keyEvent(up);
    }

    std::string switchToNgosen(fcitx::NgoSenEngine& engine, const fcitx::InputMethodEntry& entry, TestInputContext& context) {
        fcitx::InputContextEvent event(&context, fcitx::EventType::InputContextSwitchInputMethod);
        engine.activate(entry, event);
        auto list = context.inputPanel().candidateList();
        if (!list || list->size() == 0)
            return "(no offer)";
        return list->candidate(0).text().toString();
    }

    // Straight to the engine, as fcitx5 sends keys to Ngó Sen while it types English.
    void typeInEnglish(fcitx::NgoSenEngine& engine, const fcitx::InputMethodEntry& entry, TestInputContext& context, const std::string& text) {
        for (const char c : text) {
            fcitx::KeyEvent down(&context, fcitx::Key(static_cast<fcitx::KeySym>(c)), false);
            engine.keyEvent(entry, down);
            fcitx::KeyEvent up(&context, fcitx::Key(static_cast<fcitx::KeySym>(c)), true);
            engine.keyEvent(entry, up);
        }
    }

    std::string offerShown(TestInputContext& context) {
        auto list = context.inputPanel().candidateList();
        if (!list || list->size() == 0)
            return "(no offer)";
        return list->candidate(0).text().toString();
    }

    std::string pressCtrlShift(fcitx::NgoSenEngine& engine, const fcitx::InputMethodEntry& entry, TestInputContext& context) {
        const fcitx::KeyStates ctrl{fcitx::KeyState::Ctrl};
        const fcitx::KeyStates both{fcitx::KeyState::Ctrl, fcitx::KeyState::Shift};
        for (const auto& [key, release] : {std::pair{fcitx::Key(FcitxKey_Control_L), false},
                                           {fcitx::Key(FcitxKey_Shift_L, ctrl), false},
                                           {fcitx::Key(FcitxKey_Shift_L, both), true},
                                           {fcitx::Key(FcitxKey_Control_L, ctrl), true}}) {
            fcitx::KeyEvent event(&context, key, release);
            engine.keyEvent(entry, event);
        }
        auto list = context.inputPanel().candidateList();
        if (!list || list->size() == 0)
            return "(no offer)";
        return list->candidate(0).text().toString();
    }

} // namespace

int main() {
    configureTestPaths("fcitx5-ngosen-retype-after-switch");
    TestInstance        testInstance;
    fcitx::NgoSenEngine engine(&testInstance.instance);
    fcitx::RawConfig    config;
    config.setValueByPath("Mode", "Sen");
    config.setValueByPath("VietnameseOn", "True");
    config.setValueByPath("InputMethod", "Telex");
    engine.setConfig(config);
    fcitx::InputMethodEntry entry("ngosen", "Ngó Sen", "vi", "ngosen");

    auto                    context = std::make_unique<TestInputContext>(&testInstance.instance, "gedit");
    // Like a GTK field: Ngó Sen can delete through the surrounding text.
    context->setCapabilityFlags(fcitx::CapabilityFlag::SurroundingText);
    context->focusIn();
    typeWhileOff(*context, "abc tieengs vieetj");
    std::string offer = switchToNgosen(engine, entry, *context);
    check(offer == "abc tieengs vieetj → abc tiếng việt", "switching on offers the text typed before", offer);

    fcitx::InputContextEvent away(context.get(), fcitx::EventType::InputContextSwitchInputMethod);
    engine.deactivate(entry, away);
    typeWhileOff(*context, "abc tieengs vieetj");
    pressSwitchKey(*context);
    offer = switchToNgosen(engine, entry, *context);
    check(offer == "abc tieengs vieetj → abc tiếng việt", "the switch key keeps the text typed before", offer);

    fcitx::RawConfig hotkeys;
    hotkeys.setValueByPath("Hotkey/TriggerKeys/0", "Super+space");
    testInstance.instance.globalConfig().load(hotkeys, true);
    engine.deactivate(entry, away);
    typeWhileOff(*context, "abc tieengs vieetj");
    pressSwitchKey(*context, "Super+space");
    offer = switchToNgosen(engine, entry, *context);
    check(offer == "abc tieengs vieetj → abc tiếng việt", "a switch key set in fcitx5 keeps the text typed before", offer);

    engine.deactivate(entry, away);
    typeWhileOff(*context, "vieetj");
    context->focusOut();
    context->focusIn();
    offer = switchToNgosen(engine, entry, *context);
    check(offer == "(no offer)", "focus leaving the field drops the text", offer);

    // Ngó Sen alone in the group: English is its own, and its toggle key offers the retype.
    fcitx::RawConfig english;
    english.setValueByPath("VietnameseOn", "False");
    engine.setConfig(english);
    auto field = std::make_unique<TestInputContext>(&testInstance.instance, "kate");
    field->setCapabilityFlags(fcitx::CapabilityFlag::SurroundingText);
    field->focusIn();
    fcitx::InputContextEvent focus(field.get(), fcitx::EventType::InputContextFocusIn);
    engine.activate(entry, focus);
    typeInEnglish(engine, entry, *field, "abc tieengs vieetj");
    offer = pressCtrlShift(engine, entry, *field);
    check(offer == "abc tieengs vieetj → abc tiếng việt", "the toggle key offers the text typed in English", offer);

    pressCtrlShift(engine, entry, *field);
    typeInEnglish(engine, entry, *field, "tieengs ");
    g_mouse_clicked.store(true);
    typeInEnglish(engine, entry, *field, "a");
    offer = pressCtrlShift(engine, entry, *field);
    check(offer == "(no offer)", "a click in English drops the text typed before it", offer);

    // Upgrades keep Ctrl+Space, which switches while Ctrl is still down.
    fcitx::RawConfig ctrlSpace;
    ctrlSpace.setValueByPath("ToggleKey/0", "Control+space");
    ctrlSpace.setValueByPath("VietnameseOn", "False");
    engine.setConfig(ctrlSpace);
    auto writer = std::make_unique<TestInputContext>(&testInstance.instance, "kwrite");
    writer->setCapabilityFlags(fcitx::CapabilityFlag::SurroundingText);
    writer->focusIn();
    fcitx::InputContextEvent writerFocus(writer.get(), fcitx::EventType::InputContextFocusIn);
    engine.activate(entry, writerFocus);
    typeInEnglish(engine, entry, *writer, "abc tieengs vieetj");
    const fcitx::KeyStates ctrl{fcitx::KeyState::Ctrl};
    fcitx::KeyEvent        ctrlDown(writer.get(), fcitx::Key(FcitxKey_Control_L), false);
    engine.keyEvent(entry, ctrlDown);
    fcitx::KeyEvent spaceDown(writer.get(), fcitx::Key(FcitxKey_space, ctrl), false);
    engine.keyEvent(entry, spaceDown);
    fcitx::KeyEvent spaceUp(writer.get(), fcitx::Key(FcitxKey_space, ctrl), true);
    engine.keyEvent(entry, spaceUp);
    check(offerShown(*writer) == "(no offer)", "Ctrl+Space waits for Ctrl to come up before offering", offerShown(*writer));
    fcitx::KeyEvent ctrlUp(writer.get(), fcitx::Key(FcitxKey_Control_L, ctrl), true);
    engine.keyEvent(entry, ctrlUp);
    check(offerShown(*writer) == "abc tieengs vieetj → abc tiếng việt", "Ctrl+Space offers once Ctrl is up", offerShown(*writer));

    if (failures == 0)
        std::cout << "all retype-after-switch checks passed\n";
    return failures == 0 ? 0 : 1;
}
