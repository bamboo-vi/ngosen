#!/usr/bin/env python3
"""Serves the editor test pages and keeps the last report a page posts to /r.

Usage: server.py <pages dir> <report file> [port]
"""

import functools
import http.server
import json
import os
import sys
import time

pages, out = sys.argv[1], sys.argv[2]
port = int(sys.argv[3]) if len(sys.argv) > 3 else 8765


class Handler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        body = self.rfile.read(int(self.headers.get("Content-Length", 0)))
        try:
            data = json.loads(body)
            data["at"] = time.time()
            with open(out + ".tmp", "w", encoding="utf-8") as f:
                json.dump(data, f, ensure_ascii=False)
            # The runner reads the file while pages write it.
            os.replace(out + ".tmp", out)
        except ValueError:
            pass
        self.send_response(204)
        self.end_headers()

    def end_headers(self):
        self.send_header("Cache-Control", "no-store")
        super().end_headers()

    def log_message(self, *args):
        pass


http.server.ThreadingHTTPServer(
    ("127.0.0.1", port), functools.partial(Handler, directory=pages)
).serve_forever()
