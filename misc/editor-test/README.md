# Web editor test pages

These pages load common JavaScript editors and report their content to a small local server. `run.sh`
types Telex into each page and compares the result with the expected text.

| Page | Editor |
| --- | --- |
| `textarea`, `div` | plain `<textarea>`, plain `contenteditable` |
| `froala`, `quill`, `lexical`, `tiptap`, `slate`, `draft`, `ckeditor`, `tinymce`, `trix` | rich text editors |
| `codemirror`, `monaco`, `ace` | code editors |
| `xterm` | xterm.js terminal with a local echo |

The pages load pinned editor versions from public CDNs, so you need a network connection.

## Running

Run it inside an X11 session, with Ngó Sen on in Gõ Sen mode:

```sh
misc/editor-test/run.sh                              # every page, every case, Firefox
BROWSER=chromium misc/editor-test/run.sh lexical tinymce
CASES=fast RUNS=5 misc/editor-test/run.sh textarea   # repeat the fast case
```

The script needs `python3`, `xdotool` and Firefox or Chromium. It types through XTEST, which
sends the keys through the input method the same way real typing does. The cases are:

| Case | Keys | Expected |
| --- | --- | --- |
| `sentence` | `tieengs vieetj laf ngoon nguwx cuar nguwowif vieetj nam ` | `tiếng việt là ngôn ngữ của người việt nam` |
| `hmm` | `hmm ` | `hmm` |
| `back` | `toan`, space, BackSpace, `s`, space | `toán` |
| `enter` | `xin`, Return, `chaof ` | two lines: `xin`, `chào` |
| `fast` | same as `sentence`, 30 ms between keys | same as `sentence` |

The script opens a fresh browser profile for each run, because xterm.js and Monaco take F5 for themselves.

`xdotool` does not work on Wayland. To test there, drive the keys from outside the session instead,
for example with `virsh send-key` into a VM. That route cannot send keys closer together than about
75 ms, so it never triggers the timing problems the `fast` case looks for.

## Writing a page

Load `report.js`, focus the editor, and call `report(name, getText)`. `getText` must return the
editor's text with lines joined by `|`, or `null` while the editor is still loading.
