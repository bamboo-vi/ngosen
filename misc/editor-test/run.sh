#!/bin/bash
# Types Telex into web editors and checks what each editor holds. Run it inside an X11 session with
# Ngó Sen on in Gõ Sen mode; it types with xdotool, so the keys go through the input method.
#
#   misc/editor-test/run.sh [page...]          all pages when none are given
#
#   BROWSER=firefox|chromium   browser to test (default firefox)
#   CASES="sentence back"      cases to run (default all, see below)
#   DELAY=120                  ms between keys; the "fast" case always uses 30
#   RUNS=1                     times to run each case
#
# Prints one tab-separated line per run: page, case, expected, got, ok/FAIL.
set -u
here=$(cd "$(dirname "$0")" && pwd)
browser=${BROWSER:-firefox}
port=8765
work=$(mktemp -d)
report=$work/last.json
profile=$work/profile
pages=${*:-textarea div froala quill lexical tiptap slate draft ckeditor tinymce trix codemirror monaco ace xterm}

python3 "$here/server.py" "$here/pages" "$report" "$port" &
server=$!
trap 'kill $server 2>/dev/null; pkill -f "$profile" 2>/dev/null; rm -rf "$work"' EXIT

# "<" is BackSpace, "^" is Return.
keys_for() {
    case $1 in
    sentence | fast) echo "tieengs vieetj laf ngoon nguwx cuar nguwowif vieetj nam |tiếng việt là ngôn ngữ của người việt nam" ;;
    hmm) echo "hmm |hmm" ;;
    back) echo "toan <s |toán" ;;
    enter) echo "xin^chaof |xin|chào" ;;
    esac
}

field() {
    python3 -I -c 'import json, sys
try:
    print(json.load(open(sys.argv[1], encoding="utf-8")).get(sys.argv[2], ""))
except (OSError, ValueError):
    print("")' "$report" "$1"
}

# Firefox contenteditable keeps a trailing space as U+00A0; editors add zero-width characters.
normalize() {
    python3 -I -c 'import sys
s = sys.stdin.read().rstrip("\n").replace("\xa0", " ").replace("​", "").replace("﻿", "")
print("|".join(p.strip() for p in s.split("|")).strip("| "))'
}

open_page() {
    pkill -f "$profile" 2>/dev/null
    sleep 1.5
    rm -rf "$profile"
    mkdir -p "$profile"
    if [ "$browser" = firefox ]; then
        firefox --profile "$profile" --no-remote "http://127.0.0.1:$port/$1.html" >/dev/null 2>&1 &
    else
        chromium --user-data-dir="$profile" --password-store=basic --no-first-run --no-default-browser-check \
            "http://127.0.0.1:$port/$1.html" >/dev/null 2>&1 &
    fi
}

# Waits for a report from a page loaded after the last one.
wait_ready() {
    local old=$1 id t
    for ((i = 0; i < 60; i++)); do
        id=$(field id)
        t=$(field t)
        if [ -n "$id" ] && [ "$id" != "$old" ] && [[ $t != ERR* ]]; then
            echo "$id"
            return 0
        fi
        sleep 0.5
    done
    return 1
}

type_keys() {
    local text=$1 delay=$2 args=() c
    for ((i = 0; i < ${#text}; i++)); do
        c=${text:i:1}
        case $c in
        "<") args+=(BackSpace) ;;
        "^") args+=(Return) ;;
        " ") args+=(space) ;;
        *) args+=("$c") ;;
        esac
    done
    xdotool key --delay "$delay" "${args[@]}"
}

for page in $pages; do
    for c in ${CASES:-sentence hmm back enter fast}; do
        spec=$(keys_for "$c")
        keys=${spec%%|*}
        want=${spec#*|}
        delay=${DELAY:-120}
        [ "$c" = fast ] && delay=30
        for ((run = 0; run < ${RUNS:-1}; run++)); do
            old=$(field id)
            # Reopen the browser for each run: some editors take F5 for themselves.
            open_page "$page"
            if ! wait_ready "$old" >/dev/null; then
                printf '%s\t%s\t%s\t-\tNOLOAD\n' "$page" "$c" "$want"
                continue
            fi
            sleep 2
            type_keys "$keys" "$delay"
            sleep 1.5
            got=$(field t | normalize)
            verdict=FAIL
            [ "$got" = "$want" ] && verdict=ok
            printf '%s\t%s\t%s\t%s\t%s\n' "$page" "$c" "$want" "$got" "$verdict"
        done
    done
done
