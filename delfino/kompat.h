#pragma once
// Bauteile abschalten, wenn die Funktion in dieser Version der Bibliothek existiert (sonst passiert nichts)
template <class T> static auto imuSleep(T& i, int) -> decltype(i.sleep(), void()) { i.sleep(); }
template <class T> static void imuSleep(T&, long) {}
template <class T> static auto extOff(T& p, int) -> decltype(p.setExtOutput(false), void()) { p.setExtOutput(false); }
template <class T> static void extOff(T&, long) {}
