/*
 * SPDX-FileCopyrightText: 2026 Ngó Sen contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
// A tone key typed over a selected word, offered as that word with the new tone.
#include "fake-app.h"
#include "fake-engine.h"
#include "fake-host.h"

#include "ngosen-globals.h"
#include "ngosen-keysym.h"
#include "ngosen-state.h"

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using ngosen::Field;
using ngosen::Mode;
using ngosen::test::AppQuirks;
using ngosen::test::FakeApp;
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

    Field makeField(const std::string& frontend = "wayland", bool surroundingText = true) {
        Field field;
        field.frontend        = frontend;
        field.surroundingText = surroundingText;
        field.preedit         = true;
        return field;
    }

    struct Run {
        Field                                    field = makeField();
        AppQuirks                                quirks;
        Mode                                     mode = Mode::Sen;
        std::string                              typed; // Telex keys typed first
        size_t                                   anchor        = 0;
        size_t                                   cursor        = 0;
        bool                                     repeatsReport = false; // reports the old state once more, as Firefox does
        uint32_t                                 tone          = 's';
        std::vector<uint32_t>                    after; // keys once the offer shows
        std::function<void(FakeResources&)>      configure;
        std::function<void(FakeApp&, FakeLoop&)> beforeAfter;
    };

    struct Result {
        std::string offer;
        std::string text;
    };

    Result retone(const Run& run) {
        realMode = run.mode;
        realtextLen.store(0);
        g_mouse_clicked.store(false);
        FakeLoop      loop;
        FakeResources resources;
        if (run.configure)
            run.configure(resources);
        auto                owned = std::make_unique<FakeApp>(loop, run.field, run.quirks);
        FakeApp*            app   = owned.get();
        ngosen::TypingState state(&resources, std::move(owned));
        app->attach(&state);
        state.activate(run.mode, false);
        for (const char c : run.typed) {
            app->type(static_cast<uint32_t>(c));
            loop.pump(5);
        }
        loop.pump(150);
        app->select(run.anchor, run.cursor);
        loop.pump(150);
        app->type(run.tone);
        if (run.repeatsReport)
            state.surroundingUpdated();
        loop.pump(150);
        Result result;
        result.offer = app->labels().empty() ? "(no offer)" : app->labels()[0];
        if (run.beforeAfter)
            run.beforeAfter(*app, loop);
        for (const uint32_t sym : run.after) {
            app->type(sym);
            loop.pump(150);
        }
        loop.pump(300);
        result.text = app->text();
        g_mouse_clicked.store(false);
        return result;
    }

    // "abc nhanh xyz" with nhanh selected.
    Run nhanh(size_t anchor = 4, size_t cursor = 9) {
        Run run;
        run.typed  = "abc nhanh xyz";
        run.anchor = anchor;
        run.cursor = cursor;
        return run;
    }

    void testOffer() {
        auto r = retone(nhanh());
        check(r.offer == "nhanh → nhánh", "a tone key over a selected word offers it with the tone", r.offer);
        r = retone(nhanh(9, 4));
        check(r.offer == "nhanh → nhánh", "the selection may run either way", r.offer);

        Run firefox                      = nhanh();
        firefox.quirks.hidesSelectionEnd = true;
        r                                = retone(firefox);
        check(r.offer == "nhanh → nhánh", "a field that hides the selection end still offers", r.offer);
        firefox.repeatsReport = true;
        r                     = retone(firefox);
        check(r.offer == "nhanh → nhánh", "a repeated old report is not the answer", r.offer);

        Run gnome   = nhanh();
        gnome.field = makeField("ibus");
        gnome.after = {ngosen::key::Tab};
        r           = retone(gnome);
        check(r.text == "abc nhánh xyz", "a field behind GNOME Shell's IBus bridge", r.text);

        Run run   = nhanh();
        run.after = {ngosen::key::Tab};
        r         = retone(run);
        check(r.text == "abc nhánh xyz", "Tab writes the word with the tone", r.text);
        run.after = {ngosen::key::Tab, ngosen::key::Tab};
        r         = retone(run);
        check(r.offer != "(no offer)" && r.text == "abc s xyz", "Tab again puts back what was typed", r.offer + " / " + r.text);
        run.after = {ngosen::key::Escape, ngosen::key::Tab};
        r         = retone(run);
        check(r.offer != "(no offer)" && r.text == "abc s xyz", "after Esc, Tab changes nothing", r.offer + " / " + r.text);
        run.after = {ngosen::key::Tab, 'a', 's'};
        r         = retone(run);
        check(r.text == "abc nhánhá xyz", "after Tab, typing starts a new word", r.text);
        run.after = {'a'};
        r         = retone(run);
        check(r.offer != "(no offer)" && r.text == "abc sa xyz", "typing on keeps the key as typed", r.offer + " / " + r.text);
    }

    void testWords() {
        const auto offerFor = [](const std::string& typed, size_t from, size_t to, uint32_t tone, const std::function<void(FakeResources&)>& configure = {}) {
            Run run;
            run.typed     = typed;
            run.anchor    = from;
            run.cursor    = to;
            run.tone      = tone;
            run.configure = configure;
            return retone(run).offer;
        };
        auto offer = offerFor("abc nhanhj xyz", 4, 9, 's');
        check(offer == "nhạnh → nhánh", "another tone replaces the old one", offer);
        offer = offerFor("abc nhanhs xyz", 4, 9, 'z');
        check(offer == "nhánh → nhanh", "the key that removes a tone", offer);
        offer = offerFor("abc NHANH xyz", 4, 9, 's');
        check(offer == "NHANH → NHÁNH", "capitals keep their case", offer);
        offer = offerFor("abc dduwowngf xyz", 4, 9, 's');
        check(offer == "đường → đướng", "a word with đ and horns", offer);
        offer = offerFor("abc af xyz", 4, 5, 's');
        check(offer == "à → á", "a single letter that has a mark", offer);
        offer = offerFor("abc nhanh xyz", 4, 9, '1', [](FakeResources& r) { r.mutableOptions().inputMethod = "VNI"; });
        check(offer == "nhanh → nhánh", "VNI tone key", offer);
        offer = offerFor("abc nhanh xyz", 4, 9, '\'', [](FakeResources& r) { r.mutableOptions().inputMethod = "VIQR"; });
        check(offer == "nhanh → nhánh", "VIQR tone key", offer);

        offer = offerFor("abc a xyz", 4, 5, 's');
        check(offer == "(no offer)", "a bare letter gets no offer", offer);
        offer = offerFor("abc tieengs xyz", 4, 9, 's');
        check(offer == "(no offer)", "the tone the word already has gets no offer", offer);
        offer = offerFor("abc nhanh xyz", 4, 9, 'x');
        check(offer == "(no offer)", "a word the engine restores as invalid gets no offer", offer);
        offer = offerFor("abc hello xyz", 4, 9, 's');
        check(offer == "(no offer)", "an English word gets no offer", offer);
        offer = offerFor("abc nhanh xyz", 0, 9, 's');
        check(offer == "(no offer)", "two words get no offer", offer);
        offer = offerFor("abc nhanh xyz", 6, 9, 's');
        check(offer == "(no offer)", "part of a word gets no offer", offer);
        offer = offerFor("abc nhanh xyz", 9, 9, 's');
        check(offer == "(no offer)", "a key typed with nothing selected gets no offer", offer);
    }

    void testWhereNotOffered() {
        Run run   = nhanh();
        run.field = makeField("xim", false);
        run.after = {ngosen::key::Tab};
        auto r    = retone(run);
        check(r.offer == "(no offer)", "a field that reports no text gets no offer", r.offer);

        run                = nhanh();
        run.quirks.reports = false;
        r                  = retone(run);
        check(r.offer == "(no offer)", "no offer until the field reports the key replaced the word", r.offer);

        run           = nhanh();
        run.configure = [](FakeResources& r) { r.mutableOptions().retoneSelection = false; };
        run.after     = {ngosen::key::Tab};
        r             = retone(run);
        check(r.offer == "(no offer)" && r.text == "abc s xyz", "the option off: no offer, Tab goes to the app", r.offer + " / " + r.text);

        run      = nhanh();
        run.mode = Mode::Preedit;
        r        = retone(run);
        check(r.offer == "(no offer)", "Preedit mode gets no offer", r.offer);

        run.mode           = Mode::Sen;
        run.field          = makeField();
        run.field.password = true;
        r                  = retone(run);
        check(r.offer == "(no offer)", "a password field gets no offer", r.offer);
    }

    void testClickDropsOffer() {
        Run run         = nhanh();
        run.beforeAfter = [](FakeApp& app, FakeLoop& loop) {
            app.moveCursor(0);
            loop.pump(150);
        };
        run.after = {ngosen::key::Tab};
        auto r    = retone(run);
        check(r.offer != "(no offer)" && r.text == "abc s xyz", "Tab after a click elsewhere changes nothing", r.offer + " / " + r.text);
    }

} // namespace

int main() {
    testOffer();
    testWords();
    testWhereNotOffered();
    testClickDropsOffer();
    if (failures == 0)
        std::cout << "all retone checks passed\n";
    return failures == 0 ? 0 : 1;
}
