// Posts the editor's text (lines joined by "|") to the test server whenever it changes.
(function () {
  const id = Math.random().toString(36).slice(2);
  let last = null;
  window.report = function (name, get) {
    setInterval(() => {
      let t;
      try {
        t = get();
      } catch (e) {
        t = 'ERR ' + e;
      }
      if (t === undefined || t === null) t = 'ERR not ready';
      if (t !== last) {
        last = t;
        fetch('/r', { method: 'POST', body: JSON.stringify({ id, name, t }) }).catch(() => {});
      }
    }, 250);
  };
})();
