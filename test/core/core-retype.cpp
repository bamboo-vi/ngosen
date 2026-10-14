/*
 * SPDX-FileCopyrightText: 2026 Ngó Sen contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
// Text typed while another input method was on, offered for retyping when Ngó Sen comes back.
#include "fake-app.h"
#include "fake-engine.h"
#include "fake-host.h"

#include "ngosen-globals.h"
#include "ngosen-keysym.h"
#include "ngosen-state.h"
#include "ngosen-xtest.h"

#include <functional>
#include <iostream>
#include <memory>
#include <string>

using ngosen::Field;
using ngosen::Mode;
using ngosen::test::AppQuirks;
using ngosen::test::FakeApp;
using ngosen::test::FakeHost;
using ngosen::test::FakeKey;
using ngosen::test::FakeLoop;
using ngosen::test::FakeResources;

namespace {

    int  failures = 0;

    void check(bool ok, const std::string& what, const std::string& actual) {
        if (!ok) {
            std::cerr << "FAIL: " << what << "\n  got: " << actual << '\n';
            ++failures;
        }
    }

    Field makeField(const std::string& frontend, bool surroundingText) {
        Field field;
        field.frontend        = frontend;
        field.surroundingText = surroundingText;
        field.preedit         = true;
        return field;
    }

    void noteWhileOff(ngosen::TypingState& state, uint32_t sym, uint32_t states = 0) {
        FakeKey down(sym, false, states);
        state.noteKeyWhileOff(down);
        FakeKey up(sym, true, states);
        state.noteKeyWhileOff(up);
    }

    void noteWhileOff(ngosen::TypingState& state, const std::string& text) {
        for (const char c : text)
            noteWhileOff(state, static_cast<uint32_t>(c));
    }

    // Ngó Sen is switched on in a field that has text typed while it was off.
    std::string offerAfter(const std::function<void(ngosen::TypingState&)>& typeWhileOff, Field field = makeField("wayland", true)) {
        realMode = Mode::Off;
        FakeLoop      loop;
        FakeResources resources;
        field.preedit             = true;
        auto                owned = std::make_unique<FakeHost>(loop, field);
        FakeHost*           host  = owned.get();
        ngosen::TypingState state(&resources, std::move(owned));
        typeWhileOff(state);
        state.activate(Mode::Sen, false);
        state.offerRetype();
        if (!host->candidates())
            return "(no offer)";
        return host->labels().empty() ? "(empty)" : host->labels()[0];
    }

    void testOffer() {
        auto offer = offerAfter([](auto& s) { noteWhileOff(s, "tieengs vieetj"); });
        check(offer == "tieengs vieetj → tiếng việt", "offer shows the typed text and the retyped one", offer);

        offer = offerAfter([](auto& s) { noteWhileOff(s, "Vieetj Nam"); });
        check(offer == "Vieetj Nam → Việt Nam", "capitals keep their case", offer);

        offer = offerAfter([](auto& s) { noteWhileOff(s, "hello world"); });
        check(offer == "(no offer)", "text that Telex leaves alone gets no offer", offer);

        offer = offerAfter([](auto& s) {
            noteWhileOff(s, "vieetjx");
            noteWhileOff(s, ngosen::key::BackSpace);
        });
        check(offer == "vieetj → việt", "backspace removes the last noted character", offer);

        offer = offerAfter([](auto& s) {
            noteWhileOff(s, "abcd ");
            noteWhileOff(s, 'a', ngosen::modifier::Ctrl);
            noteWhileOff(s, "vieetj");
        });
        check(offer == "vieetj → việt", "a shortcut starts the text over", offer);

        offer = offerAfter([](auto& s) {
            noteWhileOff(s, "abcd ");
            noteWhileOff(s, ngosen::key::Left);
            noteWhileOff(s, "vieetj");
        });
        check(offer == "vieetj → việt", "moving the cursor starts the text over", offer);

        offer = offerAfter([](auto& s) {
            noteWhileOff(s, "vieetj");
            s.forgetKeysWhileOff();
        });
        check(offer == "(no offer)", "focus leaving the field drops the text", offer);

        // A GTK4 terminal over D-Bus: no forwarded keys, no surrounding text, and no XTEST here.
        offer = offerAfter([](auto& s) { noteWhileOff(s, "vieetj"); }, makeField("dbus", false));
        check(offer == "(no offer)", "no offer where the text cannot be deleted", offer);

        Field password    = makeField("wayland", true);
        password.password = true;
        offer             = offerAfter([](auto& s) { noteWhileOff(s, "vieetj"); }, password);
        check(offer == "(no offer)", "nothing is noted in a password field", offer);
    }

    void testFocusInDoesNotOffer() {
        realMode = Mode::Off;
        FakeLoop            loop;
        FakeResources       resources;
        auto                owned = std::make_unique<FakeHost>(loop, makeField("wayland", true));
        FakeHost*           host  = owned.get();
        ngosen::TypingState state(&resources, std::move(owned));
        noteWhileOff(state, "vieetj");
        state.activate(Mode::Sen, true);
        state.offerRetype();
        check(!host->candidates(), "focus coming in does not offer old text", "offered");
    }

    // Types text in the app as if another input method were on, switches Ngó Sen on, then presses keys.
    std::string retypeInApp(const Field& field, Mode mode, const std::string& typed, const std::vector<uint32_t>& after) {
        realMode = Mode::Off;
        realtextLen.store(0);
        FakeLoop      loop;
        FakeResources resources;
        AppQuirks     quirks;
        quirks.handsBackKeys      = field.frontend == "xim";
        auto                owned = std::make_unique<FakeApp>(loop, field, quirks);
        FakeApp*            app   = owned.get();
        ngosen::TypingState state(&resources, std::move(owned));
        app->attach(&state);
        for (const char c : typed) {
            noteWhileOff(state, static_cast<uint32_t>(c));
            app->type(static_cast<uint32_t>(c));
            loop.pump(5);
        }
        state.activate(mode, false);
        state.offerRetype();
        for (const uint32_t sym : after) {
            app->type(sym);
            loop.pump(150);
        }
        loop.pump(300);
        return app->text();
    }

    void testRetypeInApp(const std::string& frontend, bool surroundingText, Mode mode) {
        const Field       field = makeField(frontend, surroundingText);
        const std::string what  = frontend + (mode == Mode::Sen ? " sen: " : " preedit: ");
        std::string       text  = retypeInApp(field, mode, "tieengs vieetj", {ngosen::key::Tab});
        check(text == "tiếng việt", what + "Tab retypes the text with marks", text);

        text = retypeInApp(field, mode, "tieengs vieetj", {ngosen::key::Tab, ngosen::key::Tab});
        check(text == "tieengs vieetj", what + "Tab again puts the typed text back", text);

        text = retypeInApp(field, mode, "tieengs", {ngosen::key::Escape, ngosen::key::Tab});
        check(text.rfind("tieengs", 0) == 0, what + "after Esc, Tab does not retype", text);

        text = retypeInApp(field, mode, "tieengs ", {'a', ngosen::key::space});
        check(text == "tieengs a ", what + "another key leaves the text as typed", text);
    }

    void pressInState(ngosen::TypingState& state, uint32_t sym, uint32_t states = 0) {
        FakeKey down(sym, false, states);
        state.keyEvent(down);
        FakeKey up(sym, true, states);
        state.keyEvent(up);
    }

    // Chromium replaces by selecting with Shift+Left through XTEST, and those keys come back to us.
    void testRetypeOverSelection() {
        constexpr uint32_t ShiftMask = 1U << 0;
        realMode                     = Mode::Off;
        FakeLoop      loop;
        FakeResources resources;
        Field         field       = makeField("dbus", false);
        field.program             = "google-chrome";
        field.keyEventOrderFix    = true;
        auto                owned = std::make_unique<FakeHost>(loop, field);
        FakeHost*           host  = owned.get();
        ngosen::TypingState state(&resources, std::move(owned));
        host->setSystemKeysWork(true);
        setXTestSenderForTest([](int) { return true; });

        noteWhileOff(state, "tieengs vieetj");
        state.activate(Mode::Sen, false);
        state.offerRetype();
        pressInState(state, ngosen::key::Tab);
        FakeKey shiftDown(ngosen::key::Shift_R);
        state.keyEvent(shiftDown);
        for (int i = 0; i < 14; ++i)
            pressInState(state, ngosen::key::Left, ShiftMask);
        FakeKey shiftUp(ngosen::key::Shift_R, true, ShiftMask);
        state.keyEvent(shiftUp);
        loop.pump(150);
        const std::string commits = host->commits().empty() ? "(none)" : host->commits().back();
        check(commits == "tiếng việt", "selection: Tab types the retyped text over the selection", commits);

        pressInState(state, ngosen::key::Tab);
        loop.pump(20);
        check(host->systemKeys().size() == 2, "selection: Tab again selects the retyped text", std::to_string(host->systemKeys().size()) + " selections");
        setXTestSenderForTest(nullptr);
    }

} // namespace

int main() {
    testOffer();
    testFocusInDoesNotOffer();
    testRetypeInApp("wayland", true, Mode::Sen);
    testRetypeInApp("xim", false, Mode::Sen);
    testRetypeInApp("wayland", true, Mode::Preedit);
    // Deletes through the surrounding text where forwarded keys do not work.
    testRetypeInApp("other", true, Mode::Sen);
    testRetypeOverSelection();
    if (failures == 0)
        std::cout << "all retype checks passed\n";
    return failures == 0 ? 0 : 1;
}
