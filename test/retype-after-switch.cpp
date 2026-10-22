// SPDX-License-Identifier: GPL-3.0-or-later
//
// Keys go through fcitx5 while another input method is on; switching to Ngó Sen then offers to
// retype them, unless focus left the field in between.
#include "ngosen-engine.h"
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

    if (failures == 0)
        std::cout << "all retype-after-switch checks passed\n";
    return failures == 0 ? 0 : 1;
}
