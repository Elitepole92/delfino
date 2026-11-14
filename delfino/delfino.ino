// Delfino - Tamagotchi fuer den M5StickS3 (M5Unified), hochkant
// Taste A (vorne) = ausfuehren, Taste B (Seite) = weiter, B lang = Einstellungen.
// Updates holt sich der Stick per WLAN von GitHub (Einstellungen -> Update).
#include <M5Unified.h>
#include <Preferences.h>
#include <math.h>
#include <string.h>
#include <time.h>
#if __has_include("version.h")
#include "version.h"            // legt GitHub beim Bauen an
#endif
#ifndef FW_VERSION
#define FW_VERSION 0
#endif
#define DREHUNG 0               // steht das Bild auf dem Kopf: 2 eintragen
static const char* UPDATE_URL = "https://github.com/Elitepole92/delfino/releases/latest/download/";

static inline uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); }
static const uint16_t C_BG = rgb(0x0b, 0x3a, 0x5c), C_BGS = rgb(0x05, 0x1c, 0x2e), C_BGN = rgb(0x08, 0x2b, 0x47), C_PANEL = rgb(0x06, 0x22, 0x38),
  C_BODY = rgb(0x6e, 0xb6, 0xe6), C_FIN = rgb(0x3f, 0x86, 0xc0), C_BELLY = rgb(0xd8, 0xee, 0xfa),
  C_SICK = rgb(0x9c, 0xc9, 0x8f), C_SICKFIN = rgb(0x6f, 0x9e, 0x63), C_DEAD = rgb(0x8a, 0x93, 0xa3), C_DEADFIN = rgb(0x6d, 0x75, 0x84),
  C_ORCA = rgb(0x1f, 0x24, 0x2e), C_ORCAB = rgb(0xf2, 0xf5, 0xf8), C_ORCASICK = rgb(0x2f, 0x4d, 0x3a), C_MOUTHL = rgb(0xc0, 0xc8, 0xd0),
  C_SHELL = rgb(0x6b, 0x9e, 0x3c), C_SHELLD = rgb(0x4a, 0x73, 0x28), C_SKIN = rgb(0x9c, 0xc4, 0x6a), C_TBELLY = rgb(0xe8, 0xd8, 0x8c),
  C_SHELLSICK = rgb(0x8f, 0x9a, 0x5a), C_SKINSICK = rgb(0xc8, 0xd2, 0x9c),
  C_EYE = rgb(0x0a, 0x14, 0x22), C_WHITE = rgb(0xff, 0xff, 0xff), C_OK = rgb(0x5b, 0xe3, 0x7d), C_LOW = rgb(0xff, 0x5a, 0x5f),
  C_TXT = rgb(0xe8, 0xed, 0xf5), C_DIM = rgb(0x8f, 0xb4, 0xd0), C_SEL = rgb(0xff, 0xcf, 0x4a), C_CHEEK = rgb(0xff, 0x8f, 0xa3),
  C_BUBBLE = rgb(0x9f, 0xd8, 0xf5), C_PLA = rgb(0xd6, 0xec, 0xf2), C_CAP = rgb(0xff, 0x5a, 0x5f), C_BAG = rgb(0xee, 0xf1, 0xf4),
  C_FOOD = rgb(0xff, 0x9f, 0x43), C_PLAY = rgb(0xb8, 0xa4, 0xff), C_WIENER = rgb(0xe0, 0x74, 0x52), C_COOKIE = rgb(0xdd, 0xb2, 0x6a), C_SADDLE = rgb(0x9a, 0xa4, 0xb4),
  C_BALLBG = rgb(0x1c, 0x12, 0x30), C_BALLWIN = rgb(0x1a, 0x2f, 0x8a), C_HELM = rgb(0x22, 0x24, 0x2a), C_HELMB = rgb(0x48, 0x4c, 0x58), C_HELMS = rgb(0x7a, 0x80, 0x90), C_HAIR = rgb(0x3a, 0x1f, 0x2e),
  C_SKINF = rgb(0xe8, 0xb8, 0x8c), C_SHADOW = rgb(0x9b, 0x5f, 0xd0), C_LIPS = rgb(0xd0, 0x2a, 0x4a), C_GOLD = rgb(0xf2, 0xc1, 0x4e),
  C_BLOUSE = rgb(0x7b, 0x3f, 0xa0), C_VEST = rgb(0xff, 0x7a, 0x1a), C_VESTY = rgb(0xe6, 0xf2, 0x2a), C_REFL = rgb(0xd8, 0xe0, 0xe8), C_CLOTH = rgb(0x5a, 0x14, 0x2a),
  C_ORB = rgb(0x9f, 0xa8, 0xff), C_ORB2 = rgb(0xd8, 0xc8, 0xff);

struct Tama {
  float hunger, fun, energy, health;
  uint32_t age;                 // Sekunden Spielzeit
  uint32_t seed;                // Zufall (Plastik, Krankheit)
  bool sleeping, dead, sick;
  int plastic;                  // Teile im Wasser, 0..5
  int menu;
  int msgIcon;                  // Meldung als Icon, -1 = keine
  bool msgNo;                   // Icon rot durchgestrichen
  int32_t msgUntil;
  int anim, animArg;            // laufende Animation (nur Optik)
  int32_t animStart;
  uint32_t now;                 // Uhrzeit in Sekunden (Ortszeit), fuer Tag und Nacht
  int32_t coolUntil;            // kurze Pause nach Sensor-Ereignissen
  int snd;                      // Melodie, die die Hauptschleife abspielen soll (0 = keine)
  int32_t sndStart;
  int species, speed, vol, sens;   // Einstellungen: Tier, Zeitraffer 0..2 (1x/10x/50x), Lautstaerke 0..3, Sensoren 0..3
  int ui, uiSel;                // Bildschirm: 0 Spiel, 1 Einstellungen, 2 Tierwahl, 3 WLAN, 4 Update
  int req;                      // Auftrag an die Hauptschleife: 1 WLAN einrichten, 2 Update
  int batt; bool chg, wifiOk;   // vom Stick gemeldet
  int netState, netPct, netErr; // Fortschritt WLAN/Update
  int ballAns; int32_t ballAt;  // Orakel: Antwort (-1 = noch keine) und Zeitpunkt des Schuettelns
};
static Tama S;
static M5Canvas cv(&M5.Display);
static Preferences prefs;
// Pixel-Icons ('#' = Pixel)
enum { I_FORK, I_PAD, I_MOON, I_SUN, I_ROD, I_KIT, I_HEART, I_BOLT, I_CLOCK, I_BALL, I_WIENER, I_COOKIE, I_NEWG, I_FAST, I_SPK, I_WAVE, I_BATT, I_WIFI, I_DL, I_BACK, I_CHECK, I_CROSSX };
static const char* const IC_FORK[] = { "#.#.#...##.", "#.#.#..###.", "#.#.#..###.", "#####..###.", ".###...###.", "..#.....#..", "..#.....#..", "..#.....#..", "..#.....#..", nullptr };
static const char* const IC_PAD[] = { "..########..", ".##########.", "###.####.###", "##...##.#.##", "###.####.###", "############", "####....####", ".##......##.", nullptr };
static const char* const IC_MOON[] = { "....###.####", "..####....#.", ".####....#..", ".####...####", ".####.......", ".#####...##.", "..########..", "....####....", nullptr };
static const char* const IC_SUN[] = { ".....#.....", ".#.......#.", "...#####...", "..#######..", "#.#######.#", "..#######..", "...#####...", ".#.......#.", ".....#.....", nullptr };
static const char* const IC_ROD[] = { "..........##.", ".........#..#", "........#...#", ".......#....#", "......#.....#", "....###.....#", "...###....#.#", "..##.......##", ".##..........", nullptr };
static const char* const IC_KIT[] = { "....####....", "....#..#....", "############", "#####..#####", "###......###", "###......###", "#####..#####", "############", nullptr };
static const char* const IC_HEART[] = { ".##...##.", "####.####", "#########", ".#######.", "..#####..", "...###...", "....#....", nullptr };
static const char* const IC_BOLT[] = { "....##.", "...##..", "..##...", ".#####.", "...##..", "..##...", ".##....", "#......", nullptr };
static const char* const IC_CLOCK[] = { "..###..", ".#...#.", "#..#..#", "#..##.#", "#.....#", ".#...#.", "..###..", nullptr };
static const char* const IC_BALL[] = { "...####...", ".########.", "##..######", "#..#######", "##########", "##########", ".########.", "...####...", nullptr };
static const char* const IC_WIENER[] = { ".##############.", "################", ".##############.", nullptr };
static const char* const IC_COOKIE[] = { "..####..", ".######.", "##.#####", "#####.##", "###.####", "######.#", ".##.###.", "..####..", nullptr };
static const char* const IC_NEWG[] = { "..#####..", ".#.....#.", "#...#...#", "#...#...#", "#.#####.#", "#...#...#", "#...#...#", ".#.....#.", "..#####..", nullptr };
static const char* const IC_FAST[] = { "#.....#.....", "##....##....", "###...###...", "####..####..", "###...###...", "##....##....", "#.....#.....", nullptr };
static const char* const IC_SPK[] = { "....#......", "...##...#..", "..###.#..#.", "#####..#.#.", "#####..#.#.", "#####..#.#.", "..###.#..#.", "...##...#..", "....#......", nullptr };
static const char* const IC_WAVE[] = { "....###....", ".#.#...#.#.", "#..#...#..#", "#..#...#..#", "#..#...#..#", ".#.#...#.#.", "....###....", nullptr };
static const char* const IC_BATT[] = { "###########.", "#.........#.", "#.........##", "#.........##", "#.........##", "#.........#.", "###########.", nullptr };
static const char* const IC_WIFI[] = { "..#######..", ".#.......#.", "#..#####..#", "..#.....#..", "....###....", "...#...#...", ".....#.....", ".....#.....", nullptr };
static const char* const IC_DL[] = { "...###...", "...###...", "...###...", "#########", ".#######.", "..#####..", "...###...", "....#....", "#########", nullptr };
static const char* const IC_BACK[] = { "...#.....", "..##.....", ".########", "#########", ".########", "..##.....", "...#.....", nullptr };
static const char* const IC_CHECK[] = { "........#", ".......##", "#.....##.", "##...##..", ".##.##...", "..###....", "...#.....", nullptr };
static const char* const IC_CROSSX[] = { "#.....#", ".#...#.", "..#.#..", "...#...", "..#.#..", ".#...#.", "#.....#", nullptr };
static const char* const* const ICONS[] = { IC_FORK, IC_PAD, IC_MOON, IC_SUN, IC_ROD, IC_KIT, IC_HEART, IC_BOLT, IC_CLOCK, IC_BALL, IC_WIENER, IC_COOKIE,
  IC_NEWG, IC_FAST, IC_SPK, IC_WAVE, IC_BATT, IC_WIFI, IC_DL, IC_BACK, IC_CHECK, IC_CROSSX };
static int icW(int id) { return (int)strlen(ICONS[id][0]); }
static int icH(int id) { int n = 0; while (ICONS[id][n]) n++; return n; }

static const char* const NAMES[3] = { "Delfino", "Orki", "Schildi" };

static void newEgg(uint32_t seed) {
  S.hunger = 80; S.fun = 80; S.energy = 80; S.health = 100; S.age = 0; S.seed = seed ? seed : 12345;
  S.sleeping = false; S.dead = false; S.sick = false; S.plastic = 0; S.menu = 0; S.msgIcon = -1; S.msgNo = false; S.msgUntil = 0;
  S.anim = 0; S.animArg = 0; S.animStart = 0;
  S.now = 0; S.coolUntil = 0; S.snd = 0; S.sndStart = 0;
  S.species = 0; S.speed = 0; S.vol = 2; S.sens = 2;
  S.ui = 0; S.uiSel = 0; S.req = 0; S.batt = -1; S.chg = false; S.wifiOk = false; S.netState = 0; S.netPct = 0; S.netErr = 0;
  S.ballAns = -1; S.ballAt = 0;
}
static float clamp100(float v) { return v < 0 ? 0 : (v > 100 ? 100 : v); }
static int rnd() { S.seed = S.seed * 1664525u + 1013904223u; return (int)((S.seed >> 16) % 10000u); }
enum { A_NONE, A_FEED, A_PLAY, A_SLEEP, A_WAKE, A_FISH, A_MEDIC, A_NEW, A_WAVES, A_DIZZY, A_BUBBLES, A_STARTLE };
enum { S_NOPE = 12, S_ALERT, S_CLICK };
// Melodien: Paare aus Frequenz (Hz, 0 = Pause) und Dauer (ms), -1 = Ende. Index = Animation bzw. S_*
static const int16_t MEL_FEED[] = { 0, 600, 220, 70, 0, 80, 220, 70, 0, 80, 220, 70, -1 };
static const int16_t MEL_PLAY[] = { 523, 60, 659, 60, 784, 60, 1047, 90, 0, 650, 392, 50, 262, 50, 196, 80, -1 };
static const int16_t MEL_SLEEP[] = { 784, 150, 659, 150, 523, 300, -1 };
static const int16_t MEL_WAKE[] = { 523, 110, 659, 110, 784, 110, 1047, 240, -1 };
static const int16_t MEL_FISH[] = { 0, 450, 1568, 25, 0, 45, 1568, 25, 0, 45, 1568, 25, 0, 45, 1568, 25, 0, 150, 1047, 80, 1319, 140, -1 };
static const int16_t MEL_MEDIC[] = { 0, 480, 1319, 80, 0, 40, 1568, 80, 0, 40, 2093, 160, -1 };
static const int16_t MEL_NEW[] = { 523, 100, 659, 100, 784, 100, 1047, 100, 784, 100, 1047, 260, -1 };
static const int16_t MEL_WAVES[] = { 330, 120, 392, 120, 330, 120, 392, 120, 440, 200, -1 };
static const int16_t MEL_DIZZY[] = { 880, 80, 784, 80, 698, 80, 622, 80, 554, 80, 494, 80, 440, 160, -1 };
static const int16_t MEL_BUBBLES[] = { 1200, 40, 0, 60, 1500, 40, 0, 80, 1300, 40, 0, 60, 1700, 40, -1 };
static const int16_t MEL_STARTLE[] = { 1760, 50, 0, 30, 2093, 120, -1 };
static const int16_t MEL_NOPE[] = { 196, 120, 0, 50, 147, 220, -1 };
static const int16_t MEL_ALERT[] = { 1760, 70, 0, 70, 1760, 70, -1 };
static const int16_t MEL_CLICK[] = { 1800, 12, -1 };
static const int16_t* const MELS[] = { nullptr, MEL_FEED, MEL_PLAY, MEL_SLEEP, MEL_WAKE, MEL_FISH, MEL_MEDIC, MEL_NEW, MEL_WAVES, MEL_DIZZY, MEL_BUBBLES, MEL_STARTLE, MEL_NOPE, MEL_ALERT, MEL_CLICK };
static void sfx(int32_t t, int id) { S.snd = id; S.sndStart = t; }
static void say(int32_t t, int icon, bool no = false) { S.msgIcon = icon; S.msgNo = no; S.msgUntil = t + 2500; if (no) sfx(t, S_NOPE); }
static void anim(int32_t t, int a, int arg = 0) { S.anim = a; S.animArg = arg; S.animStart = t; S.msgUntil = t + 1200 + 2500; sfx(t, a); }
static int hourOf(uint32_t e) { return (int)((e / 3600u) % 24u); }
static bool isNight() { int hh = hourOf(S.now); return hh >= 22 || hh < 7; }

static void gStep() {   // genau 1 Spielsekunde
  if (S.dead) return;
  S.age += 1;
  bool night = isNight();
  if (S.sleeping) { S.energy += 0.15f; S.hunger -= 0.02f; S.fun -= 0.01f; }
  else { S.hunger -= 0.05f; S.fun -= 0.04f; S.energy -= night ? 0.06f : 0.02f; }   // nachts wird er schneller muede
  S.fun -= 0.01f * S.plastic;
  if (S.sick) S.fun -= 0.02f;
  S.hunger = clamp100(S.hunger); S.fun = clamp100(S.fun); S.energy = clamp100(S.energy);
  if (S.sleeping && S.energy >= 100 && !night) S.sleeping = false;   // nachts bleibt er liegen
  if (!S.sleeping && night && S.energy < 40) S.sleeping = true;      // und schlaeft von selbst ein
  if (rnd() < 40 && S.plastic < 5) S.plastic++;           // im Schnitt alle 4 Minuten ein Teil
  float low = fminf(S.hunger, fminf(S.fun, S.energy));
  int risk = 2 + 3 * S.plastic + (low < 25 ? 6 : 0);      // pro 10000 und Sekunde
  if (rnd() < risk && !S.sick) S.sick = true;
  if (low < 10 || S.sick) S.health -= (low < 10 ? 0.05f : 0) + (S.sick ? 0.04f : 0);
  else S.health += 0.02f;
  S.health = clamp100(S.health);
  if (S.health <= 0) { S.dead = true; S.sleeping = false; }
}
static void gFeed(int32_t t) {
  if (S.dead) return;
  if (S.sleeping) { say(t, I_MOON); return; }
  if (S.hunger > 90) { say(t, I_FORK, true); return; }
  S.hunger = clamp100(S.hunger + 25); say(t, I_HEART); anim(t, A_FEED, (int)(S.age % 2));
}
static void gPlay(int32_t t) {
  if (S.dead) return;
  if (S.sleeping) { say(t, I_MOON); return; }
  if (S.sick) { say(t, I_PAD, true); return; }
  if (S.energy < 15) { say(t, I_BOLT, true); return; }
  S.fun = clamp100(S.fun + 20); S.energy = clamp100(S.energy - 8); S.hunger = clamp100(S.hunger - 5); say(t, I_HEART); anim(t, A_PLAY);
}
static void gSleep(int32_t t) {
  if (S.dead) return;
  if (S.sleeping) { S.sleeping = false; say(t, I_SUN); anim(t, A_WAKE); }
  else { S.sleeping = true; say(t, I_MOON); anim(t, A_SLEEP); }
}
static void gFish(int32_t t) {
  if (S.dead) return;
  if (S.plastic == 0) { say(t, I_ROD, true); return; }
  S.plastic--; S.fun = clamp100(S.fun + 5); say(t, I_HEART); anim(t, A_FISH, S.plastic);
}
static void gMedic(int32_t t) {
  if (S.dead) return;
  if (!S.sick) { say(t, I_KIT, true); return; }
  S.sick = false; S.fun = clamp100(S.fun - 5); say(t, I_HEART); anim(t, A_MEDIC);
}
static void gShake(int32_t t, int lvl) {   // 1 = sanft, 2 = wild
  if (S.dead || t < S.coolUntil) return;
  S.coolUntil = t + 6000;
  if (S.sleeping) { S.sleeping = false; S.fun = clamp100(S.fun - 5); say(t, -1); anim(t, A_STARTLE); return; }
  if (lvl == 1) { S.fun = clamp100(S.fun + 10); say(t, I_HEART); anim(t, A_WAVES); }
  else { S.fun = clamp100(S.fun - 15); if (rnd() < 3333) S.sick = true; say(t, -1); anim(t, A_DIZZY); }
}
static void catchUp(uint32_t target) {      // Stick war aus: das Tier hat geschlafen, Zeit lief 10x langsamer
  if (target <= S.now) return;
  uint32_t n = (target - S.now) / 10; if (n > 60480) n = 60480;   // hoechstens eine Woche
  for (uint32_t i = 0; i < n && !S.dead; i++) { S.now += 10; S.sleeping = true; gStep(); }
  S.now = target;
  if (!S.dead && !isNight() && S.energy >= 50) S.sleeping = false;
}
static bool needs() { return !S.dead && (S.sick || S.plastic >= 3 || fminf(S.hunger, fminf(S.fun, S.energy)) < 25); }
static void newGame(int32_t t, int sp) {    // neues Tier, Einstellungen und Uhr bleiben
  uint32_t now = S.now; int speed = S.speed, vol = S.vol, sens = S.sens, batt = S.batt; bool chg = S.chg, wifiOk = S.wifiOk;
  newEgg(S.seed ^ (uint32_t)t);
  S.now = now; S.speed = speed; S.vol = vol; S.sens = sens; S.batt = batt; S.chg = chg; S.wifiOk = wifiOk; S.species = sp;
  say(t, I_HEART); anim(t, A_NEW);
}
// ---- Orakel (versteckt: im Menue hinter "Zurueck" A druecken) ----
static const char* const ANSWERS[18] = {
  "Mach erst|mal Line|Check.",
  "Das regelt|der|Kompressor.",
  "Frag den|Rigger.",
  "Nur wenn|das Gaffa|reicht.",
  "Ja, aber|nur mit|Nebel.",
  "Steck's|nochmal aus|und an.",
  "Die Antwort|liegt im|Case 17.",
  "Signal ist|da. Ton|nicht.",
  "Frag nach|dem Abbau|nochmal.",
  "Phantom-|speisung|vergessen.",
  "Das ist ein|Problem|der|Location.",
  "Sieht gut|aus. Vom|FOH aus.",
  "Ja, aber|der Kunde|will rot.",
  "Ich schau|mal im|Catering.",
  "Zeit fuer|ne Produk-|tionsbe-|sprechung.",
  "Tausch mal|das Kabel.",
  "Do it.|Fast.",
  "Frag die|Helfer.",
};
static void gBall(int32_t t) {   // schuetteln oder A: neue Antwort
  if (S.ui != 5 || (S.ballAns >= 0 && t < S.ballAt + 4000)) return;   // Pause zwischen zwei Fragen
  int a = rnd() % 18; if (a == S.ballAns) a = (a + 1) % 18;
  S.ballAns = a; S.ballAt = t; sfx(t, A_DIZZY);
}
// ---- Tasten ----
static const int SET_N = 8;   // Einstellungen: Neues Spiel, Tempo, Lautstaerke, Sensoren, Akku, WLAN, Update, Zurueck (+ versteckt: Orakel)
static void pressLong() {     // Taste B lang: Einstellungen auf/zu
  if (S.ui == 0) { S.ui = 1; S.uiSel = 0; } else if (S.ui <= 2 || S.ui == 5) S.ui = 0; else S.ui = 1;
}
static void pressNext() {
  if (S.ui == 1) S.uiSel = (S.uiSel + 1) % (SET_N + 1);   // nach "Zurueck" kommt der versteckte Platz
  else if (S.ui == 5) { S.ui = 1; S.uiSel = SET_N; }
  else if (S.ui == 2) S.uiSel = (S.uiSel + 1) % 4;
  else if (S.ui == 0 && !S.dead) S.menu = (S.menu + 1) % 5;
}
static void pressOk(int32_t t) {
  if (S.ui == 1) {
    if (S.uiSel == 0) { S.ui = 2; S.uiSel = S.species; }
    else if (S.uiSel == 1) S.speed = (S.speed + 1) % 3;
    else if (S.uiSel == 2) S.vol = (S.vol + 1) % 4;
    else if (S.uiSel == 3) S.sens = (S.sens + 1) % 4;
    else if (S.uiSel == 5) { S.ui = 3; S.req = 1; S.netState = 0; }
    else if (S.uiSel == 6) { S.ui = 4; S.req = 2; S.netState = 1; S.netPct = 0; S.netErr = 0; }
    else if (S.uiSel == 7) S.ui = 0;
    else if (S.uiSel == SET_N) { S.ui = 5; S.ballAns = -1; }
    return;
  }
  if (S.ui == 2) { if (S.uiSel == 3) { S.ui = 1; S.uiSel = 0; } else { newGame(t, S.uiSel); S.ui = 0; } return; }
  if (S.ui == 5) { gBall(t); return; }
  if (S.ui >= 3) { S.ui = 1; return; }
  if (S.dead) { newGame(t, S.species); return; }
  if (S.menu == 0) gFeed(t); else if (S.menu == 1) gPlay(t); else if (S.menu == 2) gSleep(t);
  else if (S.menu == 3) gFish(t); else gMedic(t);
}
enum Mood { M_DEAD, M_SLEEP, M_SICK, M_SAD, M_HAPPY, M_OK };
static int mood() {     // liefert ein M_* (als int, sonst stolpert die Arduino IDE)
  if (S.dead) return M_DEAD;
  if (S.sleeping) return M_SLEEP;
  if (S.sick) return M_SICK;
  float low = fminf(S.hunger, fminf(S.fun, S.energy)), avg = (S.hunger + S.fun + S.energy) / 3;
  if (low < 25) return M_SAD;
  return avg >= 60 ? M_HAPPY : M_OK;
}

// ================= Zeichnen (135 x 240, hochkant) =================
static void R(int x, int y, int w, int h, uint16_t c) { if (w > 0 && h > 0) cv.fillRect(x, y, w, h, c); }
static void T(const char* s, int x, int y, int size, uint16_t c) { cv.setTextSize(size); cv.setTextColor(c); cv.setCursor(x, y); cv.print(s); }
static int tw(const char* s, int size) { return 6 * size * (int)strlen(s); }
static void iconK(int id, int x, int y, int kn, int kd, uint16_t c) {   // Icon um kn/kd vergroessert
  for (int r = 0; ICONS[id][r]; r++)
    for (int k = 0; ICONS[id][r][k]; k++)
      if (ICONS[id][r][k] == '#') { int x0 = k * kn / kd, y0 = r * kn / kd; R(x + x0, y + y0, (k + 1) * kn / kd - x0, (r + 1) * kn / kd - y0, c); }
}
static void icon(int id, int x, int y, int s, uint16_t c) { iconK(id, x, y, s, 1, c); }
static int kW(int id, int kn, int kd) { return icW(id) * kn / kd; }
static int kH(int id, int kn, int kd) { return icH(id) * kn / kd; }
static void iconNo(int id, int x, int y, int s) {   // rot durchstreichen
  int w = icW(id), hh = icH(id);
  for (int i = 0; i < w; i++) R(x + i * s, y + (i * hh / w) * s, s + 1, s + 1, C_LOW);
}
static const int W = 135, H = 240, WTOP = 23;
static const int DCX = 68, DCY = 112;                 // Mitte des Tiers
static const int INFX = 67, INFY = 180;               // Mitte fuer Meldungen
static const int CORNX = 132, NAMEY = 194, AGEY = 176; // Name und Alter rechts unten
static const int MENUY = 214;
static const int FOODX = 112, FOODY = 30;             // Futter kommt von oben rechts
static const int PL[5][2] = { { 4, 30 }, { 118, 32 }, { 6, 140 }, { 120, 146 }, { 8, 196 } };
static const int ST[4][2] = { { 20, 40 }, { 60, 40 }, { 100, 152 }, { 30, 176 } };
static const int CIR[8][2] = { { 0, -10 }, { 7, -7 }, { 10, 0 }, { 7, 7 }, { 0, 10 }, { -7, 7 }, { -10, 0 }, { -7, -7 } };
// Gesichter der Tiere: Auge, Mund (Start, Laenge), Wange, Nase (Futterziel), Kopf oben (Ball), Atemblasen
struct Spec { int ex, ey, mx, my, ml, chx, chy, nx, ny, bx, by, hx, hy; };
static const Spec SPC[3] = {
  { 10, -6, 22, 2, 10, 12, 0, 34, 2, 24, -2, 6, -24 },      // Delfin
  { 12, -7, 16, 4, 10, 21, -1, 29, 3, 10, -14, 2, -16 },    // Orca
  { 28, -6, 30, 0, 5, 23, -3, 37, -2, 24, -8, 36, -6 } };   // Schildkroete
static void drawPlastic(int i, int x, int y) {
  if (i % 2 == 0) { R(x + 1, y, 4, 2, C_CAP); R(x, y + 2, 6, 11, C_PLA); R(x, y + 6, 6, 2, C_CAP); }
  else { R(x, y, 10, 2, C_BAG); R(x + 1, y + 2, 8, 5, C_BAG); R(x, y + 7, 3, 3, C_BAG); R(x + 7, y + 7, 3, 3, C_BAG); }
}
static int gkn = 5, gkd = 4, gcx, gcy;   // Vergroesserung und Mitte des Tiers
static int sc(int v) { int p = v * gkn; return p >= 0 ? p / gkd : -((-p + gkd - 1) / gkd); }   // abrunden
static void SR(int dx, int dy, int w, int h, uint16_t c) { int x0 = sc(dx), y0 = sc(dy); R(gcx + x0, gcy + y0, sc(dx + w) - x0, sc(dy + h) - y0, c); }

static void drawPet(int sp, int cx, int cy, int mo, bool sk, bool dead, int32_t t, int an, int ap, bool full) {
  gcx = cx; gcy = cy;
  const Spec& P = SPC[sp];
  uint16_t mc = C_EYE, eb;
  if (sp == 2) {   // Schildkroete
    uint16_t shell = dead ? C_DEAD : (sk ? C_SHELLSICK : C_SHELL), skin = dead ? C_DEADFIN : (sk ? C_SKINSICK : C_SKIN), dk = dead ? C_DEADFIN : C_SHELLD;
    SR(10, 4, 10, 5, skin); SR(14, 9, 9, 4, skin); SR(-34, 4, 10, 5, skin); SR(-39, 9, 9, 4, skin); SR(-46, -2, 10, 4, skin);
    SR(16, -5, 8, 7, skin); SR(22, -8, 14, 10, skin); SR(24, -10, 10, 2, skin);
    SR(-26, -22, 36, 4, shell); SR(-32, -18, 48, 4, shell); SR(-36, -14, 56, 8, shell); SR(-38, -6, 60, 8, shell);
    SR(-36, 2, 56, 4, dead ? C_DEADFIN : C_TBELLY);
    SR(-22, -16, 10, 6, dk); SR(-6, -18, 12, 6, dk); SR(10, -14, 8, 6, dk); SR(-30, -6, 10, 6, dk); SR(-14, -6, 12, 6, dk); SR(4, -6, 10, 6, dk);
    eb = skin;
  } else if (sp == 1) {   // Orca: schwarz-weiss, runder Kopf, hohe Rueckenflosse
    uint16_t blk = dead ? C_DEADFIN : (sk ? C_ORCASICK : C_ORCA), wht = C_ORCAB;
    SR(-40, -4, 14, 8, blk); SR(-46, -6, 6, 12, blk); SR(-52, -12, 8, 8, blk); SR(-52, 4, 8, 8, blk);
    SR(-10, -18, 10, 4, blk); SR(-9, -22, 8, 4, blk); SR(-8, -26, 6, 4, blk); SR(-7, -30, 5, 4, blk); SR(-6, -34, 4, 4, blk); SR(-5, -38, 2, 4, blk);
    SR(-30, -14, 48, 4, blk); SR(-34, -10, 56, 4, blk); SR(-36, -6, 64, 12, blk); SR(-34, 6, 58, 4, blk); SR(-26, 10, 44, 4, blk);
    SR(-22, -14, 12, 2, C_SADDLE);                                              // grauer Sattel
    SR(-28, 6, 52, 4, wht); SR(-20, 10, 36, 4, wht); SR(14, 4, 12, 2, wht); SR(-24, 0, 12, 6, wht);   // Bauch, Kinn, Flanke
    SR(7, -11, 11, 2, wht); SR(5, -9, 15, 7, wht);                              // Augenfleck
    SR(-2, 8, 10, 5, blk); SR(0, 13, 9, 4, blk);                                // Brustflosse
    eb = wht;
  } else {         // Delfin
    uint16_t body = dead ? C_DEAD : (sk ? C_SICK : C_BODY), fin = dead ? C_DEADFIN : (sk ? C_SICKFIN : C_FIN);
    SR(-44, -3, 14, 6, body); SR(-48, -4, 4, 8, fin); SR(-54, -12, 6, 8, fin); SR(-54, 4, 6, 8, fin);
    SR(-6, -22, 10, 4, fin); SR(-10, -26, 7, 4, fin); SR(-13, -30, 4, 4, fin);
    SR(-12, -18, 24, 4, body); SR(-22, -14, 40, 4, body); SR(-30, -10, 50, 4, body); SR(-34, -6, 58, 12, body);
    SR(-30, 6, 50, 4, body); SR(-22, 10, 34, 4, body); SR(24, -2, 9, 7, body);
    SR(-28, 6, 46, 4, C_BELLY); SR(-20, 10, 30, 2, C_BELLY); SR(22, 3, 11, 2, C_BELLY);
    SR(-6, 8, 8, 4, fin); SR(-8, 12, 6, 4, fin);
    eb = body;
  }
  int ex = P.ex, ey = P.ey, mx = P.mx, my = P.my, ml = P.ml;
  bool blink = (t % 3200) < 140;
  if (mo == M_DEAD) { for (int i = 0; i < 6; i += 2) { SR(ex - 1 + i, ey - 1 + i, 2, 2, C_EYE); SR(ex + 3 - i, ey - 1 + i, 2, 2, C_EYE); } }
  else if (an == A_DIZZY) { SR(ex - 1, ey - 1, 6, 6, C_EYE); SR(ex, ey, 4, 4, eb); SR(ex + 1, ey + 1, 2, 2, C_EYE); }
  else if (an == A_STARTLE) { SR(ex - 1, ey - 2, 6, 7, C_EYE); SR(ex, ey - 1, 2, 2, C_WHITE); }
  else if (mo == M_SLEEP || blink) SR(ex - 1, ey + 2, 6, 2, C_EYE);
  else { SR(ex, ey, 4, 5, C_EYE); SR(ex + 1, ey + 1, 1, 1, C_WHITE); }
  if (mo == M_SAD && !blink && an == A_NONE) SR(ex + 1, ey + 6, 2, 3, C_BUBBLE);
  if (an == A_FEED && ap >= 600) { if ((ap / 150) % 2 == 0) SR(mx + 2, my - 1, ml - 1, 3, mc); else SR(mx, my, ml, 1, mc); }
  else if (an == A_STARTLE) SR(mx + 3, my - 1, ml < 5 ? ml : 5, 3, mc);
  else if (mo == M_HAPPY) { SR(mx, my, ml, 1, mc); SR(mx - 2, my - 1, 2, 1, mc); SR(P.chx, P.chy, 5, 2, C_CHEEK); }
  else if (mo == M_OK) SR(mx, my, ml, 1, mc);
  else if (mo == M_SAD) { SR(mx, my, ml, 1, mc); SR(mx - 2, my + 1, 2, 1, mc); }
  else if (mo == M_SICK) { SR(mx, my + 1, ml / 2, 1, mc); SR(mx + ml / 2, my, ml - ml / 2, 1, mc); }
  else if (mo == M_SLEEP) SR(mx, my, ml - 2, 1, mc);
  else SR(mx, my + 1, ml, 1, mc);
  if (!full) return;
  if (sk) { SR(-46, -30, 2, 8, C_LOW); SR(-49, -27, 8, 2, C_LOW); }
  if (!S.sleeping && !dead) { int p = (t / 50) % 40; if (p < 12) R(cx + sc(P.hx) + ((p >> 2) % 2), cy + sc(P.hy) - p, 2, 2, C_BUBBLE); }
  if (mo == M_SLEEP && an != A_SLEEP) { bool z = ((t / 700) % 2) == 1; T(z ? "Z" : "z", cx + sc(34), cy + sc(-30) + (z ? 0 : 4), 2, C_TXT); }
}

static void drawHeader() {   // Hunger (Besteck), Spass (Controller), Energie (Blitz), darunter Gesundheit
  float vals[3] = { S.hunger, S.fun, S.energy };
  const int labs[3] = { I_FORK, I_PAD, I_BOLT };
  const uint16_t lcol[3] = { C_FOOD, C_PLAY, C_SEL };
  for (int i = 0; i < 3; i++) {
    int x0 = 2 + i * 44, id = labs[i];
    iconK(id, x0 + (18 - kW(id, 3, 2)) / 2, 2 + (14 - kH(id, 3, 2)) / 2, 3, 2, lcol[i]);
    R(x0 + 20, 5, 22, 8, C_PANEL);
    int w = (int)floorf(vals[i] * 0.20f + 0.5f);
    if (w > 0) R(x0 + 21, 6, w, 6, vals[i] < 25 ? C_LOW : C_OK);
  }
  int hw = (int)floorf(S.health * 1.31f + 0.5f);
  R(2, 19, 131, 2, C_PANEL);
  if (hw > 0) R(2, 19, hw, 2, S.health < 30 ? C_LOW : C_OK);
}
static void drawMenu(int an, bool sk) {
  if (S.dead) { int x = (W - 44) / 2; R(x, MENUY, 44, 22, C_SEL); iconK(I_HEART, x + (44 - kW(I_HEART, 3, 2)) / 2, MENUY + (22 - kH(I_HEART, 3, 2)) / 2, 3, 2, C_BG); return; }
  const int ids[5] = { I_FORK, I_PAD, S.sleeping ? I_SUN : I_MOON, I_ROD, I_KIT };
  const uint16_t cols[5] = { C_FOOD, C_PLAY, C_SEL, C_OK, C_CHEEK };
  for (int i = 0; i < 5; i++) {
    int x = 1 + i * 27, id = ids[i]; bool sel = S.menu == i;
    bool warn = (i == 3 && S.plastic + (an == A_FISH ? 1 : 0) >= 3) || (i == 4 && sk);
    R(x, MENUY, 26, 22, sel ? C_SEL : C_PANEL);
    iconK(id, x + (26 - kW(id, 3, 2)) / 2, MENUY + (22 - kH(id, 3, 2)) / 2, 3, 2, sel ? C_BG : cols[i]);
    if (warn) { R(x + 19, MENUY, 7, 7, C_BG); R(x + 20, MENUY + 1, 5, 5, C_LOW); }   // roter Punkt = da ist was zu tun
  }
}

static void drawGame(int32_t t) {
  int mo = mood();
  uint16_t bg = S.sleeping ? C_BGS : (isNight() ? C_BGN : C_BG);
  R(0, 0, W, H, bg);
  drawHeader();
  for (int i = 0; i < S.plastic; i++) drawPlastic(i, PL[i][0], PL[i][1] + (int)(((t / 600) + i) % 2));
  // Animation der letzten Aktion (1,2 Sekunden, nur Optik)
  int ap = t - S.animStart, an = (S.anim && ap >= 0 && ap < 1200) ? S.anim : A_NONE;
  if (an == A_FISH) {     // Angelschnur runter, Teil einhaken, hochziehen
    int i = S.animArg, sx = PL[i][0], sy = PL[i][1], d = sy - 4 - WTOP;
    int ly = ap < 500 ? WTOP + d * ap / 500 : sy - 4 - d * (ap - 500) / 700;
    R(sx + 4, WTOP, 1, ly - WTOP + 3, C_TXT); R(sx + 1, ly + 3, 4, 1, C_TXT); R(sx + 1, ly + 1, 1, 2, C_TXT);
    drawPlastic(i, sx, ap < 500 ? sy : ly + 4);   // liegt noch, bis der Haken es greift
  } else if (an == A_WAVES) {     // Wellen ziehen durchs Wasser
    for (int r = 0; r < 2; r++) for (int k = 0; k < 16; k++) R((k * 16 + ap / 20) % 256 - 16, 40 + r * 110 + ((k + r) % 2) * 2, 8, 2, C_BUBBLE);
  }
  int jy = 0, dx = 0;
  if (an == A_PLAY) jy = -((ap * (1200 - ap)) / 24000);       // Sprung
  else if (an == A_WAKE) jy = -((ap * (1200 - ap)) / 60000);  // kleiner Hopser
  else if (an == A_WAVES) jy = -(((ap / 150) % 2) * 3);       // schaukelt
  else if (an == A_STARTLE) jy = ap < 200 ? -(ap / 20) : (ap < 600 ? -10 : -((1200 - ap) / 60));
  if (an == A_DIZZY) dx = ((ap / 60) % 2) * 4 - 2;            // wackelt
  gkn = 5; gkd = 4;
  int cx = DCX + dx, bob = (!S.sleeping && !S.dead && ((t / 500) % 2) == 1) ? 2 : 0, cy = DCY + bob + jy;
  bool pend = an == A_MEDIC && ap < 500;   // Kapsel noch unterwegs: sieht noch krank aus
  if (pend && mo != M_SLEEP) mo = M_SICK;
  bool sk = (S.sick || pend) && !S.dead;
  drawPet(S.species, cx, cy, mo, sk, S.dead, t, an, ap, true);
  const Spec& P = SPC[S.species];
  if (an == A_FEED) {     // Wienerle oder Haferkeks kommt angeschwommen, dann wird geschnappt
    if (ap < 600) {
      int f = S.animArg ? I_COOKIE : I_WIENER, tx = cx + sc(P.nx), ty = cy + sc(P.ny) - kH(f, 3, 2) / 2;
      iconK(f, FOODX + (tx - FOODX) * ap / 600, FOODY + (ty - FOODY) * ap / 600, 3, 2, S.animArg ? C_COOKIE : C_WIENER);
    }
  } else if (an == A_PLAY) {   // Ball auf dem Kopf, Spritzer beim Eintauchen
    int bb = (ap % 400) * (400 - ap % 400) / 4000;
    iconK(I_BALL, cx + sc(P.bx) - 2, cy + sc(P.by) - kH(I_BALL, 3, 2) - bb, 3, 2, C_LOW);
    if (ap > 900) { int sp = (ap - 900) / 15; for (int i = 0; i < 5; i++) R(cx + sc(-34) + i * 20, cy + sc(16) + 6 - sp - (i % 2) * 4, 3, 3, C_BUBBLE); }
  } else if (an == A_SLEEP) {  // Mond sinkt herab, Sterne funkeln
    int my = cy + sc(-30);
    iconK(I_MOON, cx + sc(20), ap < 600 ? -16 + (my + 16) * ap / 600 : my, 3, 2, C_SEL);
    for (int i = 0; i < 4; i++) if (((ap / 200) + i) % 2 == 0) { R(ST[i][0] - 2, ST[i][1], 5, 1, C_SEL); R(ST[i][0], ST[i][1] - 2, 1, 5, C_SEL); }
  } else if (an == A_WAKE) {   // Sonne geht auf
    iconK(I_SUN, cx + sc(26), cy + sc(-16) - 20 * ap / 1200, 3, 2, C_SEL);
  } else if (an == A_MEDIC) {  // Kapsel faellt ins Maul, dann Funkeln
    if (ap < 500) { int px = cx + sc(P.bx + 2), pe = cy + sc(P.by - 4) - 5, py = WTOP + (pe - WTOP) * ap / 500; R(px, py, 5, 5, C_LOW); R(px + 5, py, 5, 5, C_TXT); }
    else for (int i = 0; i < 4; i++) { int x = cx + sc(-40 + i * 26), y = cy + sc(-12) - (ap - 500) / 30 - (i % 2) * 8; R(x - 2, y, 5, 1, C_OK); R(x, y - 2, 1, 5, C_OK); }
  } else if (an == A_NEW) {    // Herzchen steigen auf
    for (int i = 0; i < 3; i++) iconK(I_HEART, cx + sc(-34 + i * 30), cy + sc(-30) - ap / 40 + (i % 2) * 6, 3, 2, C_CHEEK);
  } else if (an == A_DIZZY) {  // Sterne kreisen um den Kopf
    for (int i = 0; i < 3; i++) { int k = (ap / 100 + i * 3) % 8, x = cx + sc(P.ex - 6) + CIR[k][0] * 2, y = cy + sc(-30) - 6 + CIR[k][1] / 2; R(x - 2, y, 5, 1, C_SEL); R(x, y - 2, 1, 5, C_SEL); }
  } else if (an == A_STARTLE) {   // Ausrufezeichen
    int x = cx + sc(P.nx + 2), y = cy + sc(-20); R(x, y, 3, 10, C_LOW); R(x, y + 12, 3, 3, C_LOW);
  }
  // Meldung (Icon) in der Mitte unter dem Tier
  if (an == A_NONE && t < S.msgUntil && S.msgIcon >= 0) {
    int id = S.msgIcon, x = INFX - icW(id), y = INFY - icH(id);
    icon(id, x, y, 2, C_TXT);
    if (S.msgNo) iconNo(id, x, y, 2);
  }
  // Name und Alter rechts unten
  const char* name = NAMES[S.species];
  int nw = tw(name, 2) + 4;
  R(CORNX - nw + 1, NAMEY, nw - 2, 18, C_SEL); R(CORNX - nw, NAMEY + 1, nw, 16, C_SEL);
  T(name, CORNX - nw + 3, NAMEY + 2, 2, C_BG);
  if (!S.dead) {
    char buf[12]; snprintf(buf, sizeof(buf), "%u", (unsigned)(S.age / 60));
    int cw = kW(I_CLOCK, 3, 2), ax = CORNX - tw(buf, 2) - cw - 2;
    iconK(I_CLOCK, ax, AGEY + 3, 3, 2, C_DIM); T(buf, ax + cw + 3, AGEY, 2, C_DIM);
  }
  drawMenu(an, sk);
}

static void drawBars(int x, int y, int n, uint16_t on, uint16_t off) {   // 3 Balken, n davon an
  for (int i = 0; i < 3; i++) R(x + i * 7, y + 12 - i * 4, 5, 4 + i * 4, i < n ? on : off);
}
static void backRow(int y, bool sel, int h = 26) {
  R(3, y, 129, h, sel ? C_SEL : C_PANEL);
  iconK(I_BACK, INFX - kW(I_BACK, 3, 2) / 2, y + (h - kH(I_BACK, 3, 2)) / 2, 3, 2, sel ? C_BG : C_TXT);
}
static void drawSettings(int32_t t) {
  R(0, 0, W, H, C_BGS);
  const int ids[8] = { I_NEWG, I_FAST, I_SPK, I_WAVE, I_BATT, I_WIFI, I_DL, I_BACK };
  for (int r = 0; r < SET_N; r++) {
    int y = 4 + r * 27; bool sel = S.uiSel == r;
    uint16_t fg = sel ? C_BG : C_TXT;
    if (r == 7) { backRow(y, sel, 24); continue; }
    R(3, y, 129, 24, sel ? C_SEL : C_PANEL);
    if (r == 0) T("Neu", 8, y + 5, 2, fg);   // neues Spiel
    else iconK(ids[r], 8 + (20 - kW(ids[r], 3, 2)) / 2, y + (24 - kH(ids[r], 3, 2)) / 2, 3, 2, fg);
    if (r == 0) { gkn = 1; gkd = 4; drawPet(S.species, 104, y + 14, M_HAPPY, false, false, t, A_NONE, 0, false); }
    else if (r == 1) { const char* v = S.speed == 0 ? "1x" : (S.speed == 1 ? "10x" : "50x"); T(v, 128 - tw(v, 2), y + 4, 2, fg); }
    else if (r == 2 || r == 3) { int n = r == 2 ? S.vol : S.sens; if (n == 0) iconK(I_CROSSX, 110, y + 7, 3, 2, sel ? C_BG : C_LOW); else drawBars(105, y + 5, n, fg, sel ? C_FIN : C_DIM); }
    else if (r == 4) {
      char b[16]; if (S.batt < 0) strcpy(b, "--"); else snprintf(b, sizeof(b), "%d%%", S.batt);
      int x = 128 - tw(b, 2); T(b, x, y + 4, 2, fg);
      if (S.chg) iconK(I_BOLT, x - 13, y + 6, 3, 2, sel ? C_BG : C_SEL);
    }
    else if (r == 5) iconK(S.wifiOk ? I_CHECK : I_CROSSX, 112, y + 7, 3, 2, sel ? C_BG : (S.wifiOk ? C_OK : C_LOW));
    else if (r == 6) { char b[12]; snprintf(b, sizeof(b), "v%d", FW_VERSION); T(b, 128 - tw(b, 2), y + 4, 2, fg); }
  }
  const char* cr = "(c) 2026 Daniel Lassak";
  T(cr, (W - tw(cr, 1)) / 2 + 1, 228, 1, S.uiSel == SET_N ? C_SEL : C_DIM);   // leuchtet, wenn der versteckte Platz gewaehlt ist
}
static void drawPicker(int32_t t) {   // Tier fuer das neue Spiel waehlen
  R(0, 0, W, H, C_BGS);
  for (int r = 0; r < 3; r++) {
    int y = 6 + r * 56; bool sel = S.uiSel == r;
    R(3, y, 129, 52, sel ? C_SEL : C_PANEL);
    gkn = 1; gkd = 2; drawPet(r, INFX, y + 22, M_HAPPY, false, false, t, A_NONE, 0, false);
    T(NAMES[r], INFX - tw(NAMES[r], 1) / 2, y + 41, 1, sel ? C_BG : C_TXT);
  }
  backRow(6 + 3 * 56, S.uiSel == 3);
}
static void drawWifi(int32_t t) {     // Anleitung: WLAN vom iPhone aus eintragen
  R(0, 0, W, H, C_BGS);
  iconK(I_WIFI, INFX - kW(I_WIFI, 2, 1) / 2, 10, 2, 1, C_TXT);
  const char* lines[10] = { "Am iPhone ins WLAN", "Delfino-Setup gehen.", "", "Die Seite oeffnet", "sich von selbst,", "sonst in Safari:", "192.168.4.1", "", "Dort dein WLAN", "eintragen." };
  for (int i = 0; i < 10; i++) T(lines[i], 6, 40 + i * 12, 1, (i == 1 || i == 6) ? C_SEL : C_TXT);
  if (S.netState == 3) iconK(I_CHECK, INFX - kW(I_CHECK, 2, 1) / 2, 170, 2, 1, C_OK);
  backRow(210, true);
}
static void drawUpdate(int32_t t) {
  R(0, 0, W, H, C_BGS);
  iconK(I_DL, INFX - kW(I_DL, 2, 1) / 2, 12, 2, 1, C_TXT);
  const char* msg = ""; int ic = -1; uint16_t icc = C_OK;
  if (S.netState == 1) msg = "Verbinde ...";
  else if (S.netState == 2) msg = "Lade Update";
  else if (S.netState == 5) { msg = "Schon aktuell"; ic = I_CHECK; }
  else if (S.netState == 6) { msg = "Fertig, Neustart"; ic = I_CHECK; }
  else if (S.netState == 4) {
    msg = S.netErr == 1 ? "Erst WLAN einrichten" : (S.netErr == 2 ? "WLAN nicht gefunden" : (S.netErr == 3 ? "Kein Update gefunden" : "Update fehlgeschlagen"));
    ic = I_CROSSX; icc = C_LOW;
  }
  T(msg, INFX - tw(msg, 1) / 2, 50, 1, C_TXT);
  if (S.netState == 2) { R(10, 70, 115, 10, C_PANEL); R(11, 71, 113 * S.netPct / 100, 8, C_OK); }
  if (ic >= 0) iconK(ic, INFX - kW(ic, 2, 1) / 2, 92, 2, 1, icc);
  if (S.netState != 1 && S.netState != 2 && S.netState != 6) backRow(210, true);
}
static int isq(int n) { int x = 0; while ((x + 1) * (x + 1) <= n) x++; return x; }
static void disc(int cx, int cy, int r, uint16_t c) { for (int dy = -r; dy <= r; dy++) { int w = isq(r * r - dy * dy); R(cx - w, cy + dy, 2 * w + 1, 1, c); } }
static void drawBall(int32_t t) {   // Esmeraldas VT Orakel
  R(0, 0, W, H, C_BALLBG);
  const char* title = "Esmeraldas VT Orakel"; T(title, (W - tw(title, 1)) / 2, 4, 1, C_SEL);
  int ap = t - S.ballAt; bool think = S.ballAns >= 0 && ap >= 0 && ap < 2500;
  // Sprechblase
  R(4, 14, 127, 86, C_TXT); R(2, 16, 131, 82, C_TXT); R(62, 100, 12, 3, C_TXT); R(64, 103, 8, 2, C_TXT); R(66, 105, 4, 2, C_TXT);
  if (think) { int n = (ap / 250) % 3 + 1; for (int i = 0; i < n; i++) R(52 + i * 12, 54, 6, 6, C_BG); }
  else if (S.ballAns < 0) T("?", 58, 47, 3, C_BG);
  else {
    const char* a = ANSWERS[S.ballAns]; int n = 1; for (const char* q = a; *q; q++) if (*q == '|') n++;
    int y = 57 - n * 9 + 1; const char* s = a;
    for (int i = 0; i < n; i++) {
      char line[24]; int k = 0; while (*s && *s != '|' && k < 23) line[k++] = *s++; line[k] = 0; if (*s == '|') s++;
      T(line, 67 - tw(line, 2) / 2, y + i * 18, 2, C_BG);
    }
  }
  int x = 67;
  R(x - 18, 108, 36, 4, C_HELM); R(x - 22, 112, 44, 8, C_HELM); R(x - 27, 120, 54, 4, C_HELMB); R(x - 2, 108, 4, 12, C_HELMB); R(x - 14, 111, 8, 2, C_HELMS);   // schwarzer Schutzhelm mit Glanz
  R(x - 24, 124, 8, 28, C_HAIR); R(x + 16, 124, 8, 28, C_HAIR);   // Locken
  R(x - 16, 124, 32, 26, C_SKINF); R(x - 12, 150, 24, 4, C_SKINF);   // Gesicht
  R(x - 11, 130, 7, 2, C_SHADOW); R(x + 4, 130, 7, 2, C_SHADOW);   // Lidschatten
  if ((t % 3000) < 120) { R(x - 10, 134, 5, 1, C_EYE); R(x + 5, 134, 5, 1, C_EYE); } else { R(x - 10, 132, 5, 3, C_EYE); R(x + 5, 132, 5, 3, C_EYE); }   // Augen, blinzeln
  R(x - 5, 143, 10, 2, C_LIPS);   // Lippenstift
  R(x - 23, 148, 3, 6, C_GOLD); R(x + 20, 148, 3, 6, C_GOLD);   // Ohrringe
  R(x - 5, 154, 10, 4, C_SKINF); R(x - 30, 158, 60, 44, C_BLOUSE);   // Hals, Bluse
  R(x - 30, 158, 22, 44, C_VESTY); R(x + 8, 158, 22, 44, C_VESTY);   // gelbe Warnweste
  R(x - 30, 178, 22, 3, C_REFL); R(x + 8, 178, 22, 3, C_REFL); R(x - 30, 188, 22, 3, C_REFL); R(x + 8, 188, 22, 3, C_REFL);   // Reflexstreifen
  R(0, 212, W, 28, C_CLOTH); R(0, 212, W, 2, C_GOLD);   // Tisch
  R(x - 12, 213, 24, 5, C_GOLD); R(x - 30, 210, 10, 5, C_SKINF); R(x + 20, 210, 10, 5, C_SKINF);   // Sockel, Haende
  R(8, 216, 10, 20, C_EYE); R(14, 206, 2, 10, C_EYE); R(10, 220, 6, 4, C_OK); R(10, 227, 6, 1, C_HELMB); R(10, 230, 6, 1, C_HELMB);   // Funkgeraet auf dem Tisch
  R(96, 226, 26, 7, C_REFL); R(100, 228, 18, 1, C_HELMB); R(100, 231, 18, 1, C_HELMB); R(122, 228, 3, 3, C_HELMB); R(124, 224, 7, 3, C_REFL); R(124, 232, 7, 3, C_REFL);   // Leatherman
  disc(x, 200, 15, think ? (((ap / 100) % 2) ? C_BALLWIN : C_ORB2) : C_ORB); R(x - 7, 192, 4, 4, C_WHITE);   // Kristallkugel
  if (think) for (int k = 1; k <= (ap / 150) % 3 + 1; k++) R(18 + k * 4, 204 - k * 2, 2, 4 + k * 4, C_SEL);   // Funkwellen
}
static void drawScene(int32_t t) {
  if (S.ui == 1) drawSettings(t);
  else if (S.ui == 2) drawPicker(t);
  else if (S.ui == 3) drawWifi(t);
  else if (S.ui == 4) drawUpdate(t);
  else if (S.ui == 5) drawBall(t);
  else drawGame(t);
}

static void saveState() {
  prefs.begin("delfino", false);
  prefs.putFloat("h", S.hunger); prefs.putFloat("f", S.fun); prefs.putFloat("e", S.energy); prefs.putFloat("hp", S.health);
  prefs.putUInt("age", S.age); prefs.putUInt("seed", S.seed); prefs.putBool("sl", S.sleeping); prefs.putBool("dead", S.dead);
  prefs.putBool("sick", S.sick); prefs.putInt("pl", S.plastic); prefs.putInt("menu", S.menu); prefs.putUInt("now", S.now);
  prefs.putInt("sp", S.species); prefs.putInt("spd", S.speed); prefs.putInt("vol", S.vol); prefs.putInt("sens", S.sens);
  prefs.end();
}
static bool loadState() {
  newEgg(12345);
  prefs.begin("delfino", true);
  bool had = prefs.isKey("h");
  if (had) {
    S.hunger = prefs.getFloat("h", 80); S.fun = prefs.getFloat("f", 80); S.energy = prefs.getFloat("e", 80); S.health = prefs.getFloat("hp", 100);
    S.age = prefs.getUInt("age", 0); S.seed = prefs.getUInt("seed", 12345); S.sleeping = prefs.getBool("sl", false); S.dead = prefs.getBool("dead", false);
    S.sick = prefs.getBool("sick", false); S.plastic = prefs.getInt("pl", 0); S.menu = prefs.getInt("menu", 0); S.now = prefs.getUInt("now", 0);
    S.species = prefs.getInt("sp", 0); S.speed = prefs.getInt("spd", 0); S.vol = prefs.getInt("vol", 2); S.sens = prefs.getInt("sens", 2);
  }
  prefs.end();
  return had;
}

#ifndef TAMA_TEST
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <esp_ota_ops.h>
#include <esp_sleep.h>
#include <driver/gpio.h>
// ---- Sensoren: Schwellen je Stufe (Einstellungen -> Sensoren: aus, schwach, mittel, stark) ----
#define DEBUG 0                                              // 1 = Messwerte im Seriellen Monitor (115200 Baud)
static const float SHAKE_SOFT[4] = { 0, 1.2f, 0.9f, 0.7f };   // mittlere Abweichung von 1 g ueber 1 s: sanft
static const float SHAKE_WILD[4] = { 0, 2.4f, 1.8f, 1.4f };   // ... ab hier: wild
static const uint8_t VOLUME[4] = { 0, 40, 90, 160 };
static const uint32_t TICK_MS[3] = { 1000, 100, 20 };       // Zeitraffer 1x / 10x / 50x: so oft vergeht eine Sekunde

static uint32_t lastTick, lastDraw, lastSave, lastAct, lastImu, shWin, lastAlert, lastBatt;
static int curBright = -1;
static float shSum = 0; static int shN = 0;
static int playId = -1, playIdx = -1; static uint32_t playStart;
static WebServer web(80);
static DNSServer dns;
static bool apOn = false;
static uint32_t apSince = 0, apSaved = 0;
static bool screenOn = true;
static const uint32_t DIM_MS = 10000, OFF_MS = 60000;      // Display: nach 10 s dunkler, nach 1 min aus
#define LEICHTSCHLAF 1                                      // 1 = bei aus-Display schlaeft der Prozessor (spart am meisten)
static const int BTN_A_PIN = 11, BTN_B_PIN = 12;            // Tasten des StickS3 (wecken aus dem Leichtschlaf)
static const uint8_t BRIGHT_ON = 90, BRIGHT_DIM = 20;       // Helligkeit an / gedimmt
static int idleA = 1, idleB = 1;                            // Pegel der Tasten, wenn nicht gedrueckt
static const uint32_t AP_MAX_MS = 300000;                  // Einrichtungs-WLAN spaetestens nach 5 min zu

// Uhrzeit als Sekunden seit 1970 (Ortszeit, ohne Zeitzonen-Umrechnung)
static uint32_t daysFromCivil(int y, unsigned m, unsigned d) {
  y -= m <= 2; int era = (y >= 0 ? y : y - 399) / 400; unsigned yoe = (unsigned)(y - era * 400);
  unsigned doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1, doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return (uint32_t)(era * 146097 + (int)doe - 719468);
}
static uint32_t rtcNow() {
  auto dt = M5.Rtc.getDateTime();
  return daysFromCivil(dt.date.year, dt.date.month, dt.date.date) * 86400u + dt.time.hours * 3600u + dt.time.minutes * 60u + dt.time.seconds;
}
static void rtcSet(uint32_t e) {
  time_t tt = e; struct tm tv; gmtime_r(&tt, &tv);
  m5::rtc_datetime_t dt;
  dt.date.year = tv.tm_year + 1900; dt.date.month = tv.tm_mon + 1; dt.date.date = tv.tm_mday; dt.date.weekDay = tv.tm_wday;
  dt.time.hours = tv.tm_hour; dt.time.minutes = tv.tm_min; dt.time.seconds = tv.tm_sec;
  M5.Rtc.setDateTime(dt);
}
static uint32_t buildTime() {   // Zeitpunkt, an dem der Sketch kompiliert wurde (Uhr des bauenden Computers)
  const char* mons = "JanFebMarAprMayJunJulAugSepOctNovDec"; char mon[4] = { 0 }; int d, y, hh, mi, ss;
  sscanf(__DATE__, "%3s %d %d", mon, &d, &y); sscanf(__TIME__, "%d:%d:%d", &hh, &mi, &ss);
  int m = (int)(strstr(mons, mon) - mons) / 3 + 1;
  return daysFromCivil(y, m, d) * 86400u + hh * 3600u + mi * 60u + ss;
}
static void drawNow() { drawScene((int32_t)millis()); cv.pushSprite(0, 0); }
static bool wifiSaved() { prefs.begin("wifi", true); bool ok = prefs.getString("s", "").length() > 0; prefs.end(); return ok; }

// ---- WLAN einrichten: Stick macht ein eigenes WLAN mit Eingabeseite auf ----
static const char PAGE[] = "<!doctype html><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'>"
  "<title>Delfino WLAN</title><style>body{font:17px -apple-system,sans-serif;margin:24px;max-width:420px}"
  "input,button{font:inherit;width:100%;padding:12px;margin:6px 0 14px;box-sizing:border-box;border-radius:10px;border:1px solid #bbb}"
  "button{background:#e8590c;color:#fff;border:0;font-weight:600}</style>"
  "<h2>WLAN fuer Delfino</h2><p>Damit holt sich der Stick spaeter Updates.</p>"
  "<form method=post action=/save><label>WLAN-Name</label><input name=s autocapitalize=none autocorrect=off required>"
  "<label>Passwort</label><input name=p type=password><button>Speichern</button></form>";
static void apStart() {
  WiFi.mode(WIFI_AP); WiFi.softAP("Delfino-Setup");
  dns.start(53, "*", WiFi.softAPIP());
  web.on("/", HTTP_GET, [] { web.send(200, "text/html", PAGE); });
  web.on("/save", HTTP_POST, [] {
    prefs.begin("wifi", false); prefs.putString("s", web.arg("s")); prefs.putString("p", web.arg("p")); prefs.end();
    S.wifiOk = true; S.netState = 3; apSaved = millis();
    web.send(200, "text/html", "<meta charset=utf-8><meta name=viewport content='width=device-width'><h2>Gespeichert!</h2><p>Du kannst das WLAN am iPhone jetzt wieder wechseln.</p>");
  });
  web.onNotFound([] { web.sendHeader("Location", "http://192.168.4.1/"); web.send(302, "text/plain", ""); });
  web.begin(); apOn = true; apSince = millis(); apSaved = 0;
}
static void apStop() { if (!apOn) return; web.stop(); dns.stop(); WiFi.softAPdisconnect(true); WiFi.mode(WIFI_OFF); apOn = false; }

// ---- Update von GitHub holen ----
static void updFail(int code) { S.netState = 4; S.netErr = code; WiFi.disconnect(true); WiFi.mode(WIFI_OFF); setCpuFrequencyMhz(80); }
static void doUpdate() {
  M5.Speaker.end(); setCpuFrequencyMhz(240);   // fuer die verschluesselte Verbindung schneller
  prefs.begin("wifi", true); String ssid = prefs.getString("s", ""), pass = prefs.getString("p", ""); prefs.end();
  if (ssid.length() == 0) { updFail(1); return; }
  S.netState = 1; drawNow();
  WiFi.mode(WIFI_STA); WiFi.begin(ssid.c_str(), pass.c_str());
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) delay(100);
  if (WiFi.status() != WL_CONNECTED) { updFail(2); return; }
  WiFiClientSecure cli; cli.setInsecure();
  int ver = -1;
  { HTTPClient http; http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    if (http.begin(cli, String(UPDATE_URL) + "version.txt")) { if (http.GET() == 200) ver = http.getString().toInt(); http.end(); } }
  if (DEBUG) Serial.printf("Update: online v%d, hier v%d\n", ver, FW_VERSION);
  if (ver <= 0) { updFail(3); return; }
  if (ver <= FW_VERSION) { S.netState = 5; WiFi.disconnect(true); WiFi.mode(WIFI_OFF); setCpuFrequencyMhz(80); return; }
  S.netState = 2; S.netPct = 0; drawNow();
  saveState();
  httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  httpUpdate.rebootOnUpdate(false);
  httpUpdate.onProgress([](int cur, int total) { int p = total > 0 ? (int)((int64_t)cur * 100 / total) : 0; if (p != S.netPct) { S.netPct = p; drawNow(); } });
  t_httpUpdate_return r = httpUpdate.update(cli, String(UPDATE_URL) + "firmware.bin");
  if (r == HTTP_UPDATE_OK) { S.netState = 6; drawNow(); delay(1500); ESP.restart(); }
  else { if (DEBUG) Serial.printf("Update-Fehler: %s\n", httpUpdate.getLastErrorString().c_str()); updFail(4); }
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);
  WiFi.persistent(false); WiFi.mode(WIFI_OFF);   // Funk aus, bis er gebraucht wird
  esp_ota_mark_app_valid_cancel_rollback();      // diese Version laeuft: behalten
  M5.Display.setRotation(DREHUNG);
  cv.setColorDepth(16);
  cv.createSprite(W, H);
  if (!loadState()) newEgg(esp_random());
  // Uhrzeit: kommt beim Kompilieren mit. Mit eingebauter Uhr laeuft sie auch weiter, wenn er aus ist.
  uint32_t bt = buildTime();
  bool hasRtc = M5.Rtc.isEnabled();
  if (hasRtc) {
    uint32_t clk = rtcNow();
    if (clk < bt) { clk = S.now > bt ? S.now : bt; rtcSet(clk); }   // Uhr leer (neu oder Akku ganz leer): ab letztem Stand weiter
    if (S.now == 0 || S.now > clk) S.now = clk; else catchUp(clk);  // Zeit, in der er aus war, nachholen
  } else if (S.now < bt) S.now = bt;                                // ohne Uhr steht die Zeit, solange er aus ist
  S.wifiOk = wifiSaved();
  idleA = digitalRead(BTN_A_PIN); idleB = digitalRead(BTN_B_PIN);   // Ruhepegel der Tasten merken
  setCpuFrequencyMhz(80);                                            // reicht fuers Spiel und spart Strom
  Serial.printf("Delfino v%d: eingebaute Uhr %s, Spielzeit %02d:%02d, Startgrund %d\n", FW_VERSION, hasRtc ? "gefunden" : "nicht gefunden", hourOf(S.now), (int)((S.now / 60) % 60), (int)esp_reset_reason());
  saveState();
  M5.Mic.end(); M5.Speaker.end();                 // Mikro wird nicht gebraucht, Lautsprecher nur fuer Melodien
  lastTick = lastDraw = lastSave = lastAct = lastImu = shWin = lastAlert = millis(); lastBatt = 0;
}

void loop() {
  M5.update();
  uint32_t now = millis(); int32_t t = (int32_t)now;
  // Tasten: A (vorne) = ausfuehren, B (Seite) kurz = weiter, B lang = Einstellungen
  bool a = M5.BtnA.wasPressed(), bl = M5.BtnB.wasHold(), bn = !bl && M5.BtnB.wasClicked();
  if ((a || bl || bn) && !screenOn) { a = bl = bn = false; lastAct = now; }   // Display war aus: erster Druck weckt nur
  bool any = false;
  if (a) { pressOk(t); any = true; saveState(); }
  if (bl) { pressLong(); any = true; }
  else if (bn) { pressNext(); any = true; }
  if (any) { lastAct = now; if (!S.snd) sfx(t, S_CLICK); }
  if (M5.BtnPWR.wasClicked() && screenOn && S.ui < 3) lastAct = now - OFF_MS - 1;   // Ausschalttaste kurz: Display aus, Stromsparschlaf
  // Auftraege aus den Einstellungen
  if (S.req == 1) { S.req = 0; apStart(); }
  if (apOn) {
    dns.processNextRequest(); web.handleClient();
    uint32_t m = millis();   // frisch messen: apSince/apSaved koennen juenger sein als 'now'
    if ((apSaved && m - apSaved > 5000) || m - apSince > AP_MAX_MS) S.ui = 1;   // gespeichert oder zu lange offen: zu
    if (S.ui != 3) apStop();
  }
  if (S.req == 2) { S.req = 0; doUpdate(); lastTick = millis(); }
  // Zeit (mit Zeitraffer): jede Sekunde ein Spielschritt, die Uhr laeuft mit
  uint32_t tickMs = TICK_MS[S.speed];
  int n = 0;
  while (now - lastTick >= tickMs && n < 500) { gStep(); S.now++; lastTick += tickMs; n++; }
  if (now - lastTick >= tickMs) lastTick = now;
  // Akku alle 5 s abfragen
  if (now - lastBatt >= 5000 || lastBatt == 0) { lastBatt = now; S.batt = M5.Power.getBatteryLevel(); S.chg = (int)M5.Power.isCharging() == 1; }
  // Bewegung: 50x pro Sekunde messen, jede Sekunde auswerten
  bool ballMode = S.ui == 5 && screenOn;
  bool sensing = (S.ui == 0 && S.sens > 0 && screenOn) || ballMode;   // Display aus: Sensoren aus
  if (sensing && now - lastImu >= 20) {
    lastImu = now; float ax, ay, az;
    if (M5.Imu.getAccel(&ax, &ay, &az)) { shSum += fabsf(sqrtf(ax * ax + ay * ay + az * az) - 1.0f); shN++; }
  }
  if (now - shWin >= 1000) {
    if (shN) {
      float avg = shSum / shN;
      if (DEBUG) Serial.printf("Schuetteln: Schnitt %.2f g\n", avg);
      if (ballMode) { if (avg > SHAKE_SOFT[S.sens ? S.sens : 2]) gBall(t); }
      else if (sensing) { if (avg > SHAKE_WILD[S.sens]) gShake(t, 2); else if (avg > SHAKE_SOFT[S.sens]) gShake(t, 1); }
    }
    shSum = 0; shN = 0; shWin = now;
  }
  // Ab und zu piepen, wenn er was braucht
  if (needs() && !S.sleeping && S.ui == 0 && now - lastAlert >= 60000) { sfx(t, S_ALERT); lastAlert = now; }
  // Melodie abspielen (Lautsprecher nur waehrend der Melodie an)
  if (S.snd) {
    playId = (S.vol == 0 || apOn) ? -1 : S.snd; playStart = S.sndStart; playIdx = -1; S.snd = 0;
    if (playId >= 0 && MELS[playId]) { M5.Speaker.begin(); M5.Speaker.setVolume(VOLUME[S.vol]); } else playId = -1;
  }
  if (playId >= 0) {
    const int16_t* m = MELS[playId]; int32_t e = (int32_t)(now - playStart), acc = 0; int i = 0;
    while (m[i] >= 0 && e >= acc + m[i + 1]) { acc += m[i + 1]; i += 2; }
    if (m[i] < 0) { playId = -1; M5.Speaker.end(); }
    else if (i != playIdx) { playIdx = i; if (m[i] > 0) M5.Speaker.tone(m[i], m[i + 1]); }
  }
  // Anzeige und Energiesparen: nach 10 s dunkler, nach 1 min aus (WLAN-Einrichtung und Update ausgenommen).
  // Nur ein Tastendruck weckt das Display wieder.
  bool keepOn = S.ui == 3 || S.ui == 4;
  uint32_t idle = now - lastAct;
  if (!keepOn && idle > OFF_MS) {
    if (screenOn) { M5.Display.setBrightness(0); M5.Display.sleep(); screenOn = false; curBright = 0; }
  } else {
    if (!screenOn) { M5.Display.wakeup(); screenOn = true; curBright = -1; lastDraw = 0; }
    bool moving = S.anim && (int32_t)(now - S.animStart) < 1200;
    if (now - lastDraw >= (moving ? 40u : 100u)) { drawScene(t); cv.pushSprite(0, 0); lastDraw = now; }
    int target = (!keepOn && idle > DIM_MS) ? (S.sleeping ? 5 : BRIGHT_DIM) : BRIGHT_ON;
    if (target != curBright) { M5.Display.setBrightness(target); curBright = target; }
  }
  if (now - lastSave >= 30000) { saveState(); lastSave = now; }
  if (screenOn) { delay(4); return; }   // kurze Pause: der Prozessor darf zwischendurch ruhen
#if LEICHTSCHLAF
  // Display aus: Prozessor schlaeft, wacht alle 4 s fuer die Spielzeit auf oder sofort bei Tastendruck
  if (playId < 0 && !apOn) {
    esp_sleep_enable_timer_wakeup(4000000ULL);
    gpio_wakeup_enable((gpio_num_t)BTN_A_PIN, idleA ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
    gpio_wakeup_enable((gpio_num_t)BTN_B_PIN, idleB ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    esp_light_sleep_start();
  }
#else
  delay(20);
#endif
}
#endif
