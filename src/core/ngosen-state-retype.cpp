/*
 * SPDX-FileCopyrightText: 2026 Ngó Sen contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
// TypingState: offering to retype, with marks, what was typed while another input method was on.
#include "ngosen-globals.h"
#include "ngosen-keysym.h"
#include "ngosen-log.h"
#include "ngosen-state.h"
#include "ngosen-utf8.h"

#include <string>

namespace ngosen {

    namespace {
        // Enough for a sentence; older text is dropped a word at a time.
        constexpr size_t MaxTypedWhileOff = 200;

        bool             isPrintableAscii(uint32_t sym) {
            return sym >= 0x20 && sym <= 0x7e;
        }

        // Shift only picks the character, so it does not count.
        bool isShortcut(const ngosen::KeyPress& key) {
            return (key.states() & (ngosen::modifier::Ctrl | ngosen::modifier::Alt | ngosen::modifier::Super)) != 0U;
        }
    } // namespace

    void TypingState::noteKeyWhileOff(const ngosen::KeyPress& key) {
        if (key.isRelease() || key.isModifier())
            return;
        // A click moved the cursor, so the text before it is no longer what was typed.
        if (g_mouse_clicked.exchange(false, std::memory_order_acq_rel))
            typedWhileOff_.clear();
        const uint32_t sym = key.sym();
        if (host_->field().password || isShortcut(key)) {
            typedWhileOff_.clear();
        } else if (ngosen::key::isBackspace(sym)) {
            if (!typedWhileOff_.empty())
                typedWhileOff_.pop_back();
        } else if (isPrintableAscii(sym)) {
            typedWhileOff_ += static_cast<char>(sym);
            if (typedWhileOff_.size() > MaxTypedWhileOff) {
                const size_t cut = typedWhileOff_.find(' ', typedWhileOff_.size() - MaxTypedWhileOff);
                typedWhileOff_.erase(0, cut == std::string::npos ? typedWhileOff_.size() : cut + 1);
            }
        } else {
            typedWhileOff_.clear();
        }
    }

    void TypingState::forgetKeysWhileOff() {
        typedWhileOff_.clear();
    }

    std::string TypingState::addMarks(const std::string& typed) {
        std::string out;
        ResetEngine(bambooEngine_.handle());
        for (const char c : typed) {
            const bool               processed = EngineProcessKeyEvent(bambooEngine_.handle(), static_cast<unsigned char>(c), 0) != 0U;
            ngosen::UniqueCPtr<char> commit(EnginePullCommit(bambooEngine_.handle()));
            if (commit)
                out += commit.get();
            if (!processed)
                out += c;
        }
        EngineCommitPreedit(bambooEngine_.handle());
        ngosen::UniqueCPtr<char> rest(EnginePullCommit(bambooEngine_.handle()));
        if (rest)
            out += rest.get();
        ResetEngine(bambooEngine_.handle());
        return out;
    }

    void TypingState::offerRetype() {
        const std::string typed = std::move(typedWhileOff_);
        typedWhileOff_.clear();
        retype_.reset();
        if (!bambooEngine_ || typed.empty() || (realMode != ngosen::Mode::Sen && realMode != ngosen::Mode::Preedit))
            return;
        // Without either, Tab could not remove what was typed.
        if (!canSendBackspaces() && !host_->field().surroundingText)
            return;
        std::string marked = addMarks(typed);
        if (marked == typed)
            return;
        NGOSEN_INFO("Offer retype: " + typed + " -> " + marked);
        retype_ = Retype{.typed = typed, .marked = marked};
        host_->showCandidates({typed + " → " + marked}, 1, [this](size_t) { applyRetype(); });
        host_->setStatus(host_->translate("Tab: retype in Vietnamese, Esc: keep"));
        host_->refreshPanel();
    }

    void TypingState::applyRetype() {
        if (!retype_)
            return;
        host_->hideCandidates();
        host_->refreshPanel();
        const bool         undo = retype_->applied;
        const std::string& from = undo ? retype_->marked : retype_->typed;
        const std::string& to   = undo ? retype_->typed : retype_->marked;
        if (canSendBackspaces()) {
            performReplacement(from, to);
        } else {
            const auto length = static_cast<int>(ngosen::utf8::length(from));
            host_->deleteSurrounding(-length, static_cast<unsigned int>(length));
            commit(to);
        }
        if (undo)
            retype_.reset();
        else
            retype_->applied = true;
    }

    void TypingState::dropRetype() {
        if (!retype_)
            return;
        if (!retype_->applied) {
            host_->hideCandidates();
            host_->refreshPanel();
        }
        retype_.reset();
    }

    bool TypingState::handleRetypeKey(ngosen::KeyPress& keyEvent) {
        if (keyEvent.isRelease() || keyEvent.isModifier())
            return false;
        const uint32_t sym = keyEvent.sym();
        if (sym == ngosen::key::Tab && !keyEvent.hasModifier()) {
            keyEvent.accept();
            // Tab again before the first retype reaches the app would delete the wrong text.
            if (!is_deleting_.load(std::memory_order_acquire))
                applyRetype();
            return true;
        }
        // XIM clients hand our own forwarded backspaces back; they are not the user moving on.
        if (is_deleting_.load(std::memory_order_acquire) && ngosen::key::isBackspace(sym))
            return false;
        if (sym == ngosen::key::Escape && !retype_->applied) {
            keyEvent.accept();
            dropRetype();
            return true;
        }
        dropRetype();
        return false;
    }

} // namespace ngosen
