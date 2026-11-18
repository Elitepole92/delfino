// Delfino offline: Die App liegt im Speicher des Handys und startet auch ohne Internet.
// Die Seite fragt nach dem Start (und beim Zurueckkehren) mit 'check' nach einer neuen Version:
// Ist bei GitHub eine neuere da, wird sie gespeichert und die Seite bekommt 'update' -> Hinweis.
const CACHE = 'delfino-1';
const FILES = ['./', './index.html', './icon.png', './manifest.webmanifest'];

self.addEventListener('install', (e) => {
  e.waitUntil(caches.open(CACHE).then((c) => c.addAll(FILES)));
  self.skipWaiting();
});

self.addEventListener('activate', (e) => {
  e.waitUntil((async () => {
    for (const k of await caches.keys()) if (k !== CACHE) await caches.delete(k);
    await self.clients.claim();
  })());
});

const isPage = (url) => url.pathname.endsWith('/') || url.pathname.endsWith('/index.html');

self.addEventListener('fetch', (e) => {
  const req = e.request, url = new URL(req.url);
  if (req.method !== 'GET' || url.origin !== self.location.origin) return;
  e.respondWith((async () => {
    const cache = await caches.open(CACHE);
    if (isPage(url)) return (await cache.match('./index.html')) || fetch(req);   // Seite: immer die gespeicherte
    const cached = await cache.match(req, { ignoreSearch: true });
    const fresh = fetch(req).then((res) => { if (res.ok) cache.put(req, res.clone()); return res; }).catch(() => cached);
    if (cached) { e.waitUntil(fresh); return cached; }
    return fresh;
  })());
});

self.addEventListener('notificationclick', (e) => {
  e.notification.close();
  e.waitUntil(self.clients.matchAll({ type: 'window', includeUncontrolled: true }).then((l) => (l[0] ? l[0].focus() : self.clients.openWindow('./'))));
});

self.addEventListener('message', (e) => {
  if (e.data !== 'check') return;
  e.waitUntil((async () => {
    try {
      const cache = await caches.open(CACHE);
      const res = await fetch('./index.html', { cache: 'no-store' });
      if (!res.ok) return;
      const old = await cache.match('./index.html');
      const [newText, oldText] = await Promise.all([res.clone().text(), old ? old.text() : Promise.resolve(null)]);
      if (newText !== oldText) {
        await cache.put('./index.html', res);
        const m = newText.match(/const FW_NAME = '([^']+)'/);
        if (e.source) e.source.postMessage({ type: 'update', version: m ? m[1] : '' });
      }
    } catch (err) { /* offline: einfach weiterspielen */ }
  })());
});
