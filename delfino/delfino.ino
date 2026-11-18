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
#define FW_VERSION 0              // fortlaufende Build-Nummer (nur fuer den Update-Vergleich)
#endif
#ifndef FW_NAME
#define FW_NAME "dev"            // sichtbare Version, kommt aus delfino/VERSION
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
  C_ORB = rgb(0x9f, 0xa8, 0xff), C_ORB2 = rgb(0xd8, 0xc8, 0xff), C_GLOVE = rgb(0x8a, 0x8f, 0x9c), C_GLOVE2 = rgb(0x5c, 0x61, 0x6e),
  C_HFUR = rgb(0xe8, 0xa0, 0x5a),
  C_HFURD = rgb(0xc9, 0x80, 0x40),
  C_HCREAM = rgb(0xf8, 0xe4, 0xc4),
  C_HPINK = rgb(0xff, 0x9f, 0xb3),
  C_HPINKD = rgb(0xf0, 0x6f, 0x8a),
  C_HVEST = rgb(0xff, 0x5f, 0xa8),
  C_HVESTD = rgb(0xd8, 0x3f, 0x88),
  C_HMOUTH = rgb(0x6a, 0x3a, 0x2a),
  C_HFURS = rgb(0xc8, 0xc0, 0x80),
  C_HFURSD = rgb(0xa8, 0xa0, 0x60),
  C_CASE = rgb(0x1e, 0x20, 0x26),
  C_ALU = rgb(0xc4, 0xcc, 0xd6),
  C_ALUD = rgb(0x8e, 0x96, 0xa2),
  C_GAFFA = rgb(0xd6, 0xd4, 0xc8),
  C_MARK = rgb(0x2a, 0x2a, 0x30),
  C_WHEEL = rgb(0x2a, 0x6a, 0xe0),
  C_MWALL = rgb(0x1a, 0x1f, 0x2e),
  C_MWALL2 = rgb(0x22, 0x28, 0x3a),
  C_CARPET = rgb(0x3a, 0x44, 0x5a),
  C_CARPETL = rgb(0x55, 0x60, 0x78),
  C_TRUSS = rgb(0xb0, 0xb8, 0xc4),
  C_LAMP = rgb(0x16, 0x16, 0x1a),
  C_WARM = rgb(0xff, 0xe0, 0x9a),
  C_MAG = rgb(0xff, 0x5f, 0xd0),
  C_CYAN = rgb(0x5f, 0xe8, 0xff),
  C_TV = rgb(0x0c, 0x0c, 0x10),
  C_SCR1 = rgb(0x1f, 0x6f, 0xc8),
  C_SCR2 = rgb(0x14, 0x22, 0x3a),
  C_BANNER = rgb(0xf2, 0xf4, 0xf8),
  C_DESK = rgb(0xe6, 0xe8, 0xee),
  C_DESKD = rgb(0xb8, 0xbc, 0xc8),
  C_COFFEE = rgb(0x6b, 0x3e, 0x1f),
  C_FILTER = rgb(0xe0, 0xa0, 0x50),
  C_EMBER = rgb(0xff, 0x6a, 0x2a), C_CASH = rgb(0x5a, 0xb8, 0x5a), C_CASHL = rgb(0xa8, 0xe0, 0x9a), C_NET = rgb(0xc4, 0xa8, 0x6a), C_NETD = rgb(0x6e, 0x58, 0x34);

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
  uint32_t messT;               // Medien Hamster: Art der Unordnung je Platz (2 Bit: Kabel, Becher, Konfetti)
  int ballAns; int32_t ballAt;  // Orakel: Antwort (-1 = noch keine) und Zeitpunkt des Schuettelns
  uint32_t ballUsed;            // Orakel: schon gegebene Antworten (je ein Bit)
  uint8_t blog[24];             // Akku-Verlauf: stuendlich Prozent, 255 = unbekannt, ganz rechts das Neueste
  int slpFail, deepN;           // Leichtschlaf-Fehlversuche, Anzahl Tiefschlaf-Naechte
  int first;                    // 1 = Tierwahl ohne Zurueck (erster Start oder Tier gestorben)
  int man;                      // Anleitung: 1 = nach der ersten Tierwahl zeigen, 2 = laeuft gerade (danach ins Spiel)
  int trawl, trawlTaps;         // Trawling: 0 aus, 1 Netz naht, 2 ausgewichen, 3 erwischt; Tipper
  int32_t trawlAt;
};
static Tama S;
static M5Canvas cv(&M5.Display);
static Preferences prefs;
// Pixel-Icons ('#' = Pixel)
enum { I_FORK, I_PAD, I_MOON, I_SUN, I_ROD, I_KIT, I_HEART, I_BOLT, I_CLOCK, I_BALL, I_WIENER, I_COOKIE, I_NEWG, I_FAST, I_SPK, I_WAVE, I_BATT, I_WIFI, I_DL, I_BACK, I_CHECK, I_CROSSX, I_CUP, I_CIG, I_BROOM, I_PARTY, I_HELP };
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
static const char* const IC_CUP[] = { "..#..#.....", ".#..#......", "..#..#.....", "########...", "#########..", "########.#.", "#########..", ".######....", "..####.....", nullptr };
static const char* const IC_CIG[] = { "........#.#.", ".........#..", "........#...", "############", "############", nullptr };
static const char* const IC_BROOM[] = { "........##.", ".......##..", "......##...", ".....##....", "...####....", "..######...", ".#######...", "#.#.#.#....", "#.#.#......", nullptr };
static const char* const IC_PARTY[] = { "...#...", ".#####.", "#..#...", "#..#...", ".#####.", "...#..#", "...#..#", ".#####.", "...#...", nullptr };   // Dollarzeichen
static const char* const IC_HELP[] = { ".#####.", "#.....#", "......#", ".....#.", "...##..", "...#...", ".......", "...#...", nullptr };
static const char* const* const ICONS[] = { IC_FORK, IC_PAD, IC_MOON, IC_SUN, IC_ROD, IC_KIT, IC_HEART, IC_BOLT, IC_CLOCK, IC_BALL, IC_WIENER, IC_COOKIE,
  IC_NEWG, IC_FAST, IC_SPK, IC_WAVE, IC_BATT, IC_WIFI, IC_DL, IC_BACK, IC_CHECK, IC_CROSSX, IC_CUP, IC_CIG, IC_BROOM, IC_PARTY, IC_HELP };
static int icW(int id) { return (int)strlen(ICONS[id][0]); }
static int icH(int id) { int n = 0; while (ICONS[id][n]) n++; return n; }

static const char* const NAMES[4] = { "Delfino", "Orki", "Schildi", "Medien Hamster" };

static void newEgg(uint32_t seed) {
  S.hunger = 90; S.fun = 90; S.energy = 90; S.health = 100; S.age = 0; S.seed = seed ? seed : 12345;
  S.sleeping = false; S.dead = false; S.sick = false; S.plastic = 0; S.menu = 0; S.msgIcon = -1; S.msgNo = false; S.msgUntil = 0;
  S.anim = 0; S.animArg = 0; S.animStart = 0;
  S.now = 0; S.coolUntil = 0; S.snd = 0; S.sndStart = 0;
  S.species = 0; S.speed = 0; S.vol = 2; S.sens = 2;
  S.ui = 0; S.uiSel = 0; S.req = 0; S.batt = -1; S.chg = false; S.wifiOk = false; S.netState = 0; S.netPct = 0; S.netErr = 0;
  S.ballAns = -1; S.ballAt = 0; S.ballUsed = 0; S.messT = 0;
  memset(S.blog, 255, sizeof(S.blog)); S.slpFail = 0; S.deepN = 0;
  S.first = 0; S.man = 0; S.trawl = 0; S.trawlTaps = 0; S.trawlAt = 0;
}
static float clamp100(float v) { return v < 0 ? 0 : (v > 100 ? 100 : v); }
enum { MS_CABLE, MS_CUP, MS_CONF };   // Unordnung beim Medien Hamster
static int messAt(int i) { return (int)((S.messT >> (i * 2)) & 3u); }
static void messSet(int i, int typ) { S.messT = (S.messT & ~(3u << (i * 2))) | ((uint32_t)typ << (i * 2)); }
static int rnd() { S.seed = S.seed * 1664525u + 1013904223u; return (int)((S.seed >> 16) % 10000u); }
enum { A_NONE, A_FEED, A_PLAY, A_SLEEP, A_WAKE, A_FISH, A_MEDIC, A_NEW, A_WAVES, A_DIZZY, A_BUBBLES, A_STARTLE };
enum { S_NOPE = 12, S_ALERT, S_CLICK, S_PARTY };
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
static const int16_t MEL_PARTY[] = { 523, 150, 659, 150, 784, 150, 659, 150, 523, 150, 659, 150, 784, 300, 0, 100, 880, 150, 784, 150, 659, 150, 784, 150, 1047, 450, -1 };
static const int16_t* const MELS[] = { nullptr, MEL_FEED, MEL_PLAY, MEL_SLEEP, MEL_WAKE, MEL_FISH, MEL_MEDIC, MEL_NEW, MEL_WAVES, MEL_DIZZY, MEL_BUBBLES, MEL_STARTLE, MEL_NOPE, MEL_ALERT, MEL_CLICK, MEL_PARTY };
static void sfx(int32_t t, int id) { S.snd = id; S.sndStart = t; }
static void say(int32_t t, int icon, bool no = false) { S.msgIcon = icon; S.msgNo = no; S.msgUntil = t + 2500; if (no) sfx(t, S_NOPE); }
static void anim(int32_t t, int a, int arg = 0) { S.anim = a; S.animArg = arg; S.animStart = t; S.msgUntil = t + 1200 + 2500; sfx(t, a); }
static int hourOf(uint32_t e) { return (int)((e / 3600u) % 24u); }
static bool isNight() { int hh = hourOf(S.now); return hh >= 22 || hh < 7; }

static void gStep() {   // genau 1 Spielsekunde
  if (S.dead) return;
  S.age += 1;
  bool night = isNight();
  if (S.sleeping) { S.energy += 0.15f; S.hunger -= 0.0025f; S.fun -= 0.0015f; }
  else { S.hunger -= 0.005f; S.fun -= 0.0035f; S.energy -= night ? 0.008f : 0.003f; }   // Beduerfnisse kommen langsam (Hunger nach ca. 3,5 h)
  S.fun -= 0.0015f * S.plastic;
  if (S.sick) S.fun -= 0.003f;
  S.hunger = clamp100(S.hunger); S.fun = clamp100(S.fun); S.energy = clamp100(S.energy);
  if (S.sleeping && S.energy >= 100 && !night) S.sleeping = false;   // nachts bleibt er liegen
  if (!S.sleeping && night && S.energy < 40) S.sleeping = true;      // und schlaeft von selbst ein
  if (rnd() < 3 && S.plastic < 5) { messSet(S.plastic, S.species == 3 ? MS_CABLE : ((S.age & 1) ? MS_CONF : MS_CABLE)); S.plastic++; }   // im Schnitt knapp jede Stunde ein Teil (Hamster: Kabelsalat, Konfetti gibt's nur nach der Party)
  float low = fminf(S.hunger, fminf(S.fun, S.energy));
  int risk = S.species == 3 ? (S.fun < 30 ? 4 : 0) + (S.plastic >= 3 ? S.plastic : 0) : (S.plastic >= 3 ? 2 * S.plastic : 0) + (low < 25 ? 4 : 0);   // pro 10000 und Sekunde; gut versorgt und sauber: wird nicht krank (Hamster: Langeweile)
  if (rnd() < risk && !S.sick) S.sick = true;
  // Gesundheit: je schlechter es ihm geht, desto schneller geht sie runter (alles leer + krank: ca. 2 h)
  float need = 0;
  if (S.hunger < 25) need += (25 - S.hunger) / 25;
  if (S.fun < 25) need += (25 - S.fun) / 25;
  if (S.energy < 25) need += (25 - S.energy) / 25;
  if (need > 0 || S.sick) S.health -= 0.004f * need + (S.sick ? 0.003f : 0);
  else S.health += 0.01f;
  S.health = clamp100(S.health);
  if (S.health <= 0) { S.dead = true; S.sleeping = false; }
}
static void gFeed(int32_t t) {
  if (S.dead) return;
  if (S.sleeping) { say(t, I_MOON); return; }
  if (S.hunger > 90) { say(t, S.species == 3 ? I_CUP : I_FORK, true); return; }
  S.hunger = clamp100(S.hunger + 25); say(t, I_HEART); anim(t, A_FEED, (int)(S.age % 2));
  if (S.species == 3) S.energy = clamp100(S.energy + 8);   // Kaffee macht wach
  if (S.species == 3 && S.plastic < 5) { messSet(S.plastic, MS_CUP); S.plastic++; }   // leerer Kaffeebecher bleibt liegen
}
static void gPlay(int32_t t) {
  if (S.dead) return;
  if (S.sleeping) { say(t, I_MOON); return; }
  if (S.species == 3) { S.fun = clamp100(S.fun + 20); S.hunger = clamp100(S.hunger - 5); say(t, I_HEART); anim(t, A_PLAY); return; }   // Zigarettenpause: immer moeglich, kostet keine Kraft
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
  if (S.plastic == 0) { say(t, S.species == 3 ? I_BROOM : I_ROD, true); return; }
  S.plastic--; S.fun = clamp100(S.fun + 5); say(t, I_HEART); anim(t, A_FISH, S.plastic + messAt(S.plastic) * 8);   // Platz + Art * 8
}
static void gMedic(int32_t t) {
  if (S.dead) return;
  if (S.species == 3) {   // Medien Hamster: Party statt Tablette (geht immer, macht gesund, hinterlaesst Konfetti)
    if (S.sleeping) { say(t, I_MOON); return; }
    S.sick = false; S.fun = clamp100(S.fun + 15); S.energy = clamp100(S.energy - 10); S.hunger = clamp100(S.hunger - 5);
    if (S.plastic < 5) { messSet(S.plastic, MS_CONF); S.plastic++; }
    say(t, I_HEART); anim(t, A_MEDIC); sfx(t, S_PARTY); S.msgUntil = t + 3000 + 2500; return;   // Party dauert 3 s
  }
  if (!S.sick) { say(t, I_KIT, true); return; }
  S.sick = false; S.fun = clamp100(S.fun - 5); say(t, I_HEART); anim(t, A_MEDIC);
}
static void gShake(int32_t t, int lvl) {   // 1 = sanft, 2 = wild
  if (S.dead || t < S.coolUntil) return;
  S.coolUntil = t + 6000;
  if (S.sleeping) { S.sleeping = false; S.fun = clamp100(S.fun - 5); say(t, -1); anim(t, A_STARTLE); return; }
  if (lvl == 1) { S.fun = clamp100(S.fun + 10); say(t, I_HEART); anim(t, A_WAVES); }
  else { S.fun = clamp100(S.fun - 15); int rs = rnd(); if (S.species != 3 && rs < 3333) S.sick = true; say(t, -1); anim(t, A_DIZZY); }
}
static const int TRAWL_MS = 3600, TRAWL_NEED = 10, TRAWL_END = 1400;
static void trawlStart(int32_t t) {   // Schleppnetz naht (nur Wassertiere, nur im Spiel)
  if (S.dead || S.sleeping || S.species == 3 || S.trawl || S.ui != 0) return;
  S.trawl = 1; S.trawlAt = t; S.trawlTaps = 0; S.msgIcon = -1; sfx(t, S_ALERT);
}
static void trawlTick(int32_t t) {
  if (!S.trawl) return;
  int32_t ap = t - S.trawlAt;
  if (S.trawl == 1 && ap >= TRAWL_MS) {
    if (S.trawlTaps >= TRAWL_NEED) { S.trawl = 2; S.fun = clamp100(S.fun + 15); say(t, I_HEART); sfx(t, A_WAKE); }
    else { S.trawl = 3; S.health = clamp100(S.health - 20); S.fun = clamp100(S.fun - 20); say(t, -1); sfx(t, A_DIZZY); }
  } else if (S.trawl >= 2 && ap >= TRAWL_MS + TRAWL_END) S.trawl = 0;
}
static void catchUp(uint32_t target) {      // Stick war aus: das Tier hat geschlafen, Zeit lief 10x langsamer
  if (target <= S.now) return;
  uint32_t n = (target - S.now) / 10; if (n > 60480) n = 60480;   // hoechstens eine Woche
  if (n == 0) { S.now = target; return; }                        // kaum Zeit vergangen: nichts aendern
  for (uint32_t i = 0; i < n && !S.dead; i++) { S.now += 10; S.sleeping = true; gStep(); }
  S.now = target;
  if (!S.dead && !isNight() && S.energy >= 50) S.sleeping = false;
}
static bool needs() { return !S.dead && (S.sick || S.plastic >= 3 || fminf(S.hunger, fminf(S.fun, S.energy)) < 25); }
static void newGame(int32_t t, int sp) {    // neues Tier, Einstellungen und Uhr bleiben
  uint32_t now = S.now; int speed = S.speed, vol = S.vol, sens = S.sens, batt = S.batt, sf = S.slpFail, dn = S.deepN; bool chg = S.chg, wifiOk = S.wifiOk;
  uint8_t bl[24]; memcpy(bl, S.blog, sizeof(bl));
  newEgg(S.seed ^ (uint32_t)t);
  memcpy(S.blog, bl, sizeof(bl)); S.slpFail = sf; S.deepN = dn;
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
static uint32_t mix32(uint32_t x) { x ^= x >> 16; x *= 2246822519u; x ^= x >> 13; x *= 3266489917u; x ^= x >> 16; return x; }
static void gBall(int32_t t) {   // schuetteln oder A: neue Antwort
  if (S.ui != 5 || (S.ballAns >= 0 && t < S.ballAt + 4000)) return;   // Pause zwischen zwei Fragen
  // wie ein gemischter Stapel: erst alle Antworten je einmal, dann neu mischen (die letzte nicht gleich wieder)
  const int NA = 18;
  if (S.ballUsed == (1u << NA) - 1) S.ballUsed = S.ballAns >= 0 ? (1u << S.ballAns) : 0;
  int left = 0; for (int i = 0; i < NA; i++) if (!((S.ballUsed >> i) & 1)) left++;
  uint32_t tt = (uint32_t)t;   // Zeitpunkt des Schuettelns mischt mit: nie dieselbe Reihenfolge
  int k = (int)(mix32(S.seed ^ (tt * 2654435761u)) % (uint32_t)left), a = 0;
  S.seed = mix32(S.seed + 0x9e3779b9u + tt);
  for (int i = 0; i < NA; i++) if (!((S.ballUsed >> i) & 1)) { if (k == 0) { a = i; break; } k--; }
  S.ballUsed |= 1u << a;
  S.ballAns = a; S.ballAt = t; sfx(t, A_DIZZY);
}
// ---- Tasten ----
static const int SET_N = 9;   // Einstellungen: Neues Spiel, Tempo, Lautstaerke, Sensoren, Akku, WLAN, Update, Zurueck (+ versteckt: Orakel)
static void closeManual() { if (S.man == 2) { S.ui = 0; S.man = 0; } else { S.ui = 1; S.uiSel = 7; } }   // nach der ersten Tierwahl ins Spiel, sonst zurueck ins Menue
static void pressLong() {     // Taste B lang: Einstellungen auf/zu
  if (S.ui == 0 && S.trawl) return;
  if (S.ui == 0) { S.ui = 1; S.uiSel = 0; } else if (S.ui <= 2 || S.ui == 5) S.ui = 0; else if (S.ui == 7) closeManual(); else S.ui = 1;   // aus Akku-Seite (6) zurueck in die Einstellungen
}
static void pressNext() {
  if (S.ui == 0 && S.trawl) return;
  if (S.ui == 1) S.uiSel = (S.uiSel + 1) % (SET_N + 1);   // nach "Zurueck" kommt der versteckte Platz
  else if (S.ui == 5) { S.ui = 1; S.uiSel = SET_N; }
  else if (S.ui == 2) S.uiSel = (S.uiSel + 1) % (S.first ? 4 : 5);
  else if (S.ui == 6) S.uiSel = (S.uiSel + 1) % 2;
  else if (S.ui == 7) S.uiSel = (S.uiSel + 1) % 3;
  else if (S.ui == 0 && !S.dead) S.menu = (S.menu + 1) % 5;
}
static void pressOk(int32_t t) {
  if (S.ui == 0 && S.trawl) { if (S.trawl == 1 && S.trawlTaps < 40) S.trawlTaps++; return; }   // Trawling: schnell tippen
  if (S.ui == 1) {
    if (S.uiSel == 0) { S.ui = 2; S.uiSel = S.species; }
    else if (S.uiSel == 1) S.speed = (S.speed + 1) % 3;
    else if (S.uiSel == 2) S.vol = (S.vol + 1) % 4;
    else if (S.uiSel == 3) S.sens = (S.sens + 1) % 4;
    else if (S.uiSel == 4) { S.ui = 6; S.uiSel = 1; }   // Akku-Seite, Auswahl steht auf Zurueck
    else if (S.uiSel == 5) { S.ui = 3; S.req = 1; S.netState = 0; }
    else if (S.uiSel == 6) { S.ui = 4; S.req = 2; S.netState = 1; S.netPct = 0; S.netErr = 0; }
    else if (S.uiSel == 7) { S.ui = 7; S.uiSel = 0; S.man = 0; }   // Anleitung
    else if (S.uiSel == 8) S.ui = 0;
    else if (S.uiSel == SET_N) { S.ui = 5; S.ballAns = -1; }
    return;
  }
  if (S.ui == 2) { if (S.uiSel == 4) { S.ui = 1; S.uiSel = 0; } else { int man = S.man; newGame(t, S.uiSel); if (man) { S.ui = 7; S.uiSel = 0; S.man = 2; } else S.ui = 0; } return; }
  if (S.ui == 5) { gBall(t); return; }
  if (S.ui == 7) { if (S.uiSel < 2) S.uiSel++; else closeManual(); return; }
  if (S.ui == 6) { if (S.uiSel == 0) S.req = 3; else { S.ui = 1; S.uiSel = 4; } return; }   // Aus / Zurueck
  if (S.ui >= 3) { S.ui = 1; return; }
  if (S.dead) { S.ui = 2; S.uiSel = S.species; S.first = 1; return; }   // Neues Spiel: zur Tierauswahl
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
struct Spec { int ex, ey, mx, my, ml, chx, chy, nx, ny, bx, by, hx, hy, ty, qx; };   // ty: Kopf oben, qx: Ausrufezeichen
static const Spec SPC[4] = {
  { 10, -6, 22, 2, 10, 12, 0, 34, 2, 24, -2, 6, -24, -30, 36 },      // Delfin
  { 12, -7, 16, 4, 10, 21, -1, 29, 3, 10, -14, 2, -16, -30, 31 },    // Orca
  { 28, -6, 30, 0, 5, 23, -3, 37, -2, 24, -8, 36, -6, -30, 39 },     // Schildkroete
  { 0, -70, 0, -56, 8, 0, 0, 0, -56, -2, -52, 0, 0, -86, 28 } };     // Medien Hamster
static void drawPlastic(int i, int x, int y) {
  if (i % 2 == 0) { R(x + 1, y, 4, 2, C_CAP); R(x, y + 2, 6, 11, C_PLA); R(x, y + 6, 6, 2, C_CAP); }
  else { R(x, y, 10, 2, C_BAG); R(x + 1, y + 2, 8, 5, C_BAG); R(x, y + 7, 3, 3, C_BAG); R(x + 7, y + 7, 3, 3, C_BAG); }
}
static int gkn = 5, gkd = 4, gcx, gcy;   // Vergroesserung und Mitte des Tiers
static bool gParty = false, gRot = false, gFlip = false; static int gHip = 0;   // Pole-Dance: ohne Case, Popo wackelt
static int sc(int v) { int p = v * gkn; return p >= 0 ? p / gkd : -((-p + gkd - 1) / gkd); }   // abrunden
static void SR(int dx, int dy, int w, int h, uint16_t c) {
  if (gRot) { int ox = dy + 43, oy = -(dx + w), ow = h; h = w; w = ow; dx = ox; dy = oy; }   // liegt auf der Seite (Hamster tot)
  else if (gFlip) dy = -(dy + h);                                                           // auf dem Ruecken (Wassertiere tot)
  int x0 = sc(dx), y0 = sc(dy); R(gcx + x0, gcy + y0, sc(dx + w) - x0, sc(dy + h) - y0, c);
}
static int isqrtI(int n) { int q = 0; while ((q + 1) * (q + 1) <= n) q++; return q; }
static void SEr(int x, int y, int rx, int ry, uint16_t c, int a, int b) {   // gefuellte Ellipse, nur Zeilen a..b
  for (int d = a; d <= b; d++) { int w = isqrtI(rx * rx * (ry * ry - d * d) / (ry * ry)); R(x - w, y + d, 2 * w + 1, 1, c); }
}
static void SE(int x, int y, int rx, int ry, uint16_t c) { SEr(x, y, rx, ry, c, -ry, ry); }

// ---- Medien Hamster (Tier 3) und sein Messestand: Tabellen { x, y, Breite, Hoehe, Farbe } ----
enum { HC_FUR, HC_FURD, HC_CREAM, HC_PINK, HC_PINKD, HC_VEST, HC_VESTD, HC_REFL, HC_EYE, HC_WHITE, HC_MOUTH, HC_HELM, HC_HELMB, HC_HELMS, HC_CASE, HC_ALU, HC_ALUD, HC_GAFFA, HC_MARK, HC_WHEEL };
static const int16_t HAM[][5] = {
  { -31, 28, 6, 3, HC_ALUD },
  { 25, 28, 6, 3, HC_ALUD },
  { -32, 31, 8, 6, HC_WHEEL },
  { -31, 30, 6, 8, HC_WHEEL },
  { -29, 33, 2, 2, HC_ALU },
  { 24, 31, 8, 6, HC_WHEEL },
  { 25, 30, 6, 8, HC_WHEEL },
  { 27, 33, 2, 2, HC_ALU },
  { -34, 2, 68, 26, HC_CASE },
  { -34, 2, 68, 2, HC_ALU },
  { -34, 26, 68, 2, HC_ALU },
  { -34, 2, 2, 26, HC_ALU },
  { 32, 2, 2, 26, HC_ALU },
  { -35, 1, 5, 5, HC_ALUD },
  { 30, 1, 5, 5, HC_ALUD },
  { -35, 24, 5, 5, HC_ALUD },
  { 14, 12, 10, 6, HC_ALU },
  { 16, 14, 6, 2, HC_ALUD },
  { 30, 24, 5, 5, HC_ALUD },
  { -17, -52, 34, 2, HC_FUR },
  { -21, -50, 42, 2, HC_FUR },
  { -24, -48, 48, 4, HC_FUR },
  { -26, -44, 52, 30, HC_FUR },
  { -27, -14, 54, 8, HC_FUR },
  { -25, -6, 50, 5, HC_FUR },
  { -30, -44, 6, 16, HC_FUR },
  { 24, -44, 6, 16, HC_FUR },
  { -30, -30, 6, 3, HC_FURD },
  { 24, -30, 6, 3, HC_FURD },
  { -22, -50, 12, 2, HC_VEST },
  { 10, -50, 12, 2, HC_VEST },
  { -24, -48, 18, 38, HC_VEST },
  { 6, -48, 18, 38, HC_VEST },
  { -6, -42, 12, 32, HC_VEST },
  { -6, -50, 12, 4, HC_CREAM },
  { -4, -46, 8, 4, HC_CREAM },
  { -2, -42, 4, 3, HC_CREAM },
  { -1, -39, 2, 29, HC_VESTD },
  { -24, -31, 22, 3, HC_REFL },
  { 2, -31, 22, 3, HC_REFL },
  { -24, -23, 22, 3, HC_REFL },
  { 2, -23, 22, 3, HC_REFL },
  { -25, -10, 50, 1, HC_FURD },
  { -1, -9, 2, 7, HC_FURD },
  { -24, -6, 18, 12, HC_FURD },
  { -23, -7, 16, 12, HC_FUR },
  { -22, -8, 14, 1, HC_FUR },
  { 6, -6, 18, 12, HC_FURD },
  { 7, -7, 16, 12, HC_FUR },
  { 8, -8, 14, 1, HC_FUR },
  { -20, -2, 10, 7, HC_PINK },
  { 10, -2, 10, 7, HC_PINK },
  { -21, -6, 3, 3, HC_PINKD },
  { -17, -7, 3, 3, HC_PINKD },
  { -13, -6, 3, 3, HC_PINKD },
  { 9, -6, 3, 3, HC_PINKD },
  { 13, -7, 3, 3, HC_PINKD },
  { 17, -6, 3, 3, HC_PINKD },
  { -27, -79, 8, 8, HC_FUR },
  { 19, -79, 8, 8, HC_FUR },
  { -25, -77, 4, 4, HC_PINK },
  { 21, -77, 4, 4, HC_PINK },
  { -14, -78, 28, 2, HC_FUR },
  { -18, -76, 36, 2, HC_FUR },
  { -21, -74, 42, 4, HC_FUR },
  { -22, -70, 44, 14, HC_FUR },
  { -21, -56, 42, 2, HC_FUR },
  { -18, -54, 36, 2, HC_FUR },
  { -25, -66, 8, 11, HC_FUR },
  { 17, -66, 8, 11, HC_FUR },
  { -23, -64, 5, 7, HC_CREAM },
  { 18, -64, 5, 7, HC_CREAM },
  { -9, -61, 18, 9, HC_CREAM },
  { -2, -60, 4, 3, HC_PINKD },
  { -11, -91, 22, 2, HC_HELM },
  { -15, -89, 30, 3, HC_HELM },
  { -18, -86, 36, 8, HC_HELM },
  { -22, -78, 44, 3, HC_HELMB },
  { -2, -91, 4, 13, HC_HELMB },
  { -12, -86, 6, 2, HC_HELMS },
  { -14, -84, 3, 2, HC_HELMS },
};
static const uint16_t HPAL[20] = { C_HFUR, C_HFURD, C_HCREAM, C_HPINK, C_HPINKD, C_HVEST, C_HVESTD, C_REFL, C_EYE, C_WHITE, C_HMOUTH, C_HELM, C_HELMB, C_HELMS, C_CASE, C_ALU, C_ALUD, C_GAFFA, C_MARK, C_WHEEL };
static uint16_t hcol(int k, bool sk, bool dead) {
  if (dead && k == HC_FUR) return C_DEAD;
  if (dead && (k == HC_FURD || k == HC_PINK || k == HC_PINKD)) return C_DEADFIN;
  if (sk && k == HC_FUR) return C_HFURS;
  if (sk && k == HC_FURD) return C_HFURSD;
  return HPAL[k];
}
enum { MC_WALL, MC_WALL2, MC_CARPET, MC_CARPETL, MC_TRUSS, MC_TV, MC_SCR1, MC_SCR2, MC_BAR1, MC_BAR2, MC_BAR3, MC_BANNER, MC_PINKV, MC_BLUE, MC_MARK, MC_ALUD, MC_DESK, MC_DESKD, MC_LAMP, MC_SWHITE };
static const int16_t MESSE[][5] = {
  { 0, 23, 135, 150, MC_WALL },
  { 0, 23, 135, 40, MC_WALL2 },
  { 0, 172, 135, 40, MC_CARPET },
  { 0, 172, 135, 2, MC_CARPETL },
  { 0, 27, 135, 2, MC_TRUSS },
  { 0, 34, 135, 2, MC_TRUSS },
  { 8, 36, 1, 20, MC_TRUSS },
  { 26, 36, 1, 20, MC_TRUSS },
  { 2, 56, 31, 20, MC_TV },
  { 4, 58, 27, 16, MC_SCR1 },
  { 7, 61, 12, 2, MC_SWHITE },
  { 7, 65, 18, 1, MC_SWHITE },
  { 7, 68, 14, 1, MC_SWHITE },
  { 108, 36, 1, 20, MC_TRUSS },
  { 126, 36, 1, 20, MC_TRUSS },
  { 102, 56, 31, 20, MC_TV },
  { 104, 58, 27, 16, MC_SCR2 },
  { 107, 68, 4, 4, MC_BAR1 },
  { 113, 64, 4, 8, MC_BAR2 },
  { 119, 61, 4, 11, MC_BAR3 },
  { 125, 66, 4, 6, MC_BAR1 },
  { 3, 170, 18, 3, MC_ALUD },
  { 5, 92, 14, 78, MC_BANNER },
  { 5, 96, 14, 6, MC_PINKV },
  { 5, 104, 14, 2, MC_BLUE },
  { 7, 112, 10, 1, MC_MARK },
  { 7, 115, 8, 1, MC_MARK },
  { 5, 150, 14, 14, MC_BLUE },
  { 112, 140, 23, 34, MC_DESK },
  { 112, 140, 23, 3, MC_DESKD },
  { 116, 120, 17, 13, MC_TV },
  { 117, 121, 15, 10, MC_SCR1 },
  { 123, 133, 3, 5, MC_LAMP },
  { 119, 137, 11, 2, MC_LAMP },
  { 119, 124, 8, 1, MC_SWHITE },
};
static const uint16_t MPAL[20] = { C_MWALL, C_MWALL2, C_CARPET, C_CARPETL, C_TRUSS, C_TV, C_SCR1, C_SCR2, C_OK, C_SEL, C_LOW, C_BANNER, C_HVEST, C_SCR1, C_MARK, C_ALUD, C_DESK, C_DESKD, C_LAMP, C_WHITE };
static const int HDCY = 150;                                            // Hamster: Oberkante Case
static const int PLH[5][2] = { { 4, 178 }, { 20, 192 }, { 106, 178 }, { 122, 190 }, { 40, 196 } };   // Muell auf dem Messeteppich
static void drawMesse(bool off) {   // Messestand (off: Hamster schlaeft, Licht und Bildschirme aus)
  for (unsigned i = 0; i < sizeof(MESSE) / sizeof(MESSE[0]); i++) {
    int k = MESSE[i][4]; bool scr = k == MC_SCR1 || k == MC_SCR2 || k == MC_BAR1 || k == MC_BAR2 || k == MC_BAR3 || k == MC_SWHITE;
    R(MESSE[i][0], MESSE[i][1], MESSE[i][2], MESSE[i][3], (off && scr) ? C_TV : MPAL[k]);
  }
  for (int i = 0; i < 17; i++) { R(i * 8, 29, 1, 5, C_TRUSS); R(i * 8 + 2, 30, 1, 1, C_TRUSS); R(i * 8 + 4, 31, 1, 1, C_TRUSS); R(i * 8 + 6, 32, 1, 1, C_TRUSS); }   // Traverse
  const int lx[3] = { 24, 67, 110 }; const uint16_t lc[3] = { C_WARM, C_MAG, C_CYAN };
  for (int i = 0; i < 3; i++) {   // Scheinwerfer
    int x = lx[i]; uint16_t c = off ? C_LAMP : lc[i];
    R(x - 1, 36, 3, 3, C_LAMP); R(x - 6, 39, 12, 2, C_LAMP); R(x - 6, 39, 2, 9, C_LAMP); R(x + 4, 39, 2, 9, C_LAMP); R(x - 4, 42, 8, 8, C_LAMP); R(x - 3, 49, 6, 2, c);
    if (!off) { R(x - 1, 53, 2, 1, c); R(x - 2, 57, 1, 1, c); R(x + 1, 61, 1, 1, c); }
  }
}
static void drawMess(int typ, int x, int y, int left) {   // Kabelsalat, leerer Becher, Konfetti (left: wie viel Konfetti noch liegt)
  if (typ == MS_CABLE) { R(x, y + 3, 12, 1, C_LAMP); R(x + 2, y, 1, 7, C_LAMP); R(x + 2, y, 6, 1, C_LAMP); R(x + 7, y, 1, 5, C_LAMP); R(x + 4, y + 5, 6, 1, C_LAMP); R(x, y + 6, 4, 1, C_LAMP); R(x + 12, y + 2, 3, 3, C_ALU); }
  else if (typ == MS_CUP) { R(x, y + 1, 7, 2, C_COFFEE); R(x + 1, y + 3, 5, 8, C_WHITE); R(x + 1, y + 6, 5, 2, C_FILTER); }
  else {
    const int px[6] = { 0, 4, 8, 2, 6, 10 }, py[6] = { 2, 0, 3, 6, 5, 7 }; const uint16_t cc[6] = { C_MAG, C_SEL, C_CYAN, C_OK, C_GAFFA, C_LOW };
    for (int k = 0; k < left; k++) R(x + px[k], y + py[k], k == 4 ? 3 : 2, k == 4 ? 1 : 2, cc[k]);
  }
}
static void cleanUp(int typ, int sx, int sy, int ap) {   // Aufraeumen beim Medien Hamster
  if (typ == MS_CABLE) {   // Kabel aufrollen: Knaeuel weg, Kabelrolle waechst
    if (ap < 600) drawMess(MS_CABLE, sx, sy, 6);
    int n = 1 + ap / 300; if (n > 4) n = 4; int ox = sx + 6, oy = sy + 4;
    for (int k = 0; k < n; k++) { int s = 2 + 2 * k; R(ox - k - 1, oy - k - 1, s + 2, 1, C_LAMP); R(ox - k - 1, oy + k + 1, s + 2, 1, C_LAMP); R(ox - k - 1, oy - k - 1, 1, s + 2, C_LAMP); R(ox + k + 1, oy - k - 1, 1, s + 2, C_LAMP); }
  } else if (typ == MS_CUP) {   // Becher fliegt in den Muellsack
    int bx = sx > 90 ? sx - 14 : sx + 14, by = sy - 4;
    R(bx, by + 2, 10, 11, C_LAMP); R(bx + 3, by, 4, 3, C_LAMP); R(bx + 2, by + 4, 2, 6, C_TV);
    if (ap < 700) drawMess(MS_CUP, sx + (bx - sx) * ap / 700, sy - (ap * (700 - ap)) / 12250, 6);
  } else {   // Konfetti wegfegen
    int left = 6 - ap / 150; if (left > 0) drawMess(MS_CONF, sx, sy, left);
    int bx = sx - 4 + ((ap / 100) % 2) * 8, by = sy - 22;
    for (int k = 0; k < 11; k++) R(bx + 12 - k, by + k * 2, 2, 2, C_FILTER);   // Stiel
    R(bx - 4, by + 23, 14, 6, C_SEL); for (int k = 0; k < 4; k++) R(bx - 3 + k * 3, by + 26, 1, 3, C_FILTER);   // Borsten
  }
}
static void partyLights(int ap) {   // Party: Scheinwerfer schiessen bunte Strahlen
  const int lx[3] = { 24, 67, 110 }; const uint16_t pc[3] = { C_MAG, C_CYAN, C_WARM };
  for (int i = 0; i < 3; i++) { uint16_t col = pc[(ap / 200 + i) % 3]; int dir = ((ap / 300 + i) % 3) - 1; for (int k = 0; k < 12; k++) R(lx[i] + dir * k * 3, 52 + k * 7, 2, 2, col); }
}
static void partyPole(int cx) {   // dicke dunkle Stange, hinter dem Hamster
  int px = cx + 38;
  R(px - 2, 36, 10, 3, C_ALUD); R(px, 36, 6, 173, C_HELM); R(px + 1, 36, 1, 173, C_HELMB); R(px - 4, 209, 14, 3, C_ALUD);
}
static void partyFront(int cx, int cy, int ap) {   // Pole-Dance: Discokugel mit Strahlen, Dollarscheine, Noten
  int by = ap < 800 ? 36 + 44 * ap / 800 : 80;   // Discokugel faehrt runter und dreht sich
  R(cx, 36, 1, by - 36 + 1, C_TRUSS);
  for (int dy = -9; dy <= 9; dy += 3) for (int dx = -9; dx <= 9; dx += 3) if (dx * dx + dy * dy <= 90) R(cx + dx - 1, by + 10 + dy - 1, 3, 3, (((dx + 9) / 3 + (dy + 9) / 3 + ap / 120) % 2) ? C_REFL : C_HELMB);
  R(cx - 5, by + 4, 2, 2, C_WHITE);
  const int rdx[6] = { -1, -2, -1, 1, 2, 1 }, rdy[6] = { 0, -1, -1, -1, -1, 0 }, r0[6] = { 12, 7, 8, 8, 7, 12 };
  for (int i = 0; i < 6; i++) { int L = 2 + ((ap / 120 + i) % 2) * 3; for (int s = 0; s < L; s++) R(cx + rdx[i] * (r0[i] + s), by + 10 + rdy[i] * (r0[i] + s), 1, 1, (i % 2) ? C_WARM : C_WHITE); }   // Strahlen oben um die Kugel
  for (int k = 0; k < 8; k++) {   // Dollarscheine: von der Seite zugeworfen, Wurfparabel (erst hoch, dann Schwerkraft)
    int ph = (ap + k * 210) % 1680, u = ph / 17;
    int sx = (k % 2) ? 146 : -12, tx = 44 + (k * 11) % 36;
    int x = sx + (tx - sx) * u / 100, y = 96 + (k * 13) % 40 - 70 * u / 100 + 150 * u * u / 10000;
    if ((ph / 90) % 2) { R(x, y, 10, 5, C_CASHL); R(x + 1, y + 1, 8, 3, C_CASH); R(x + 4, y + 1, 2, 3, C_CASHL); }   // Schein trudelt: mal flach, mal hochkant
    else { R(x + 2, y - 1, 6, 7, C_CASHL); R(x + 3, y, 4, 5, C_CASH); R(x + 4, y + 2, 2, 1, C_CASHL); }
  }
  for (int i = 0; i < 2; i++) { int nx = cx - 36 + i * 10, ny = cy - 100 - ((ap / 30 + i * 15) % 20); R(nx, ny, 1, 6, C_TXT); R(nx - 2, ny + 5, 3, 2, C_TXT); R(nx + 1, ny, 2, 1, C_TXT); }
}
static void mug(int x, int y, int ap) {   // Kaffeetasse mit Dampf
  R(x, y, 9, 10, C_WHITE); R(x + 1, y + 1, 7, 2, C_COFFEE); R(x + 9, y + 2, 3, 1, C_WHITE); R(x + 11, y + 2, 1, 5, C_WHITE); R(x + 9, y + 6, 3, 1, C_WHITE);
  for (int i = 0; i < 3; i++) R(x + 2 + i * 3, y - 3 - ((ap / 120 + i) % 4), 1, 2, C_DESKD);
}
// Name des Hamsters: wie mit Schablone aufs Case gesprueht (eigene 3x5-Schrift)
static const char* const HW_CH = "MEDINHASTR";
static const char* const HW[10][5] = {
  { "#...#", "##.##", "#.#.#", "#...#", "#...#" },   // M
  { "###", "#..", "##.", "#..", "###" },   // E
  { "##.", "#.#", "#.#", "#.#", "##." },   // D
  { "#", "#", "#", "#", "#" },   // I
  { "#..#", "##.#", "#.##", "#..#", "#..#" },   // N
  { "#.#", "#.#", "###", "#.#", "#.#" },   // H
  { ".#.", "#.#", "###", "#.#", "#.#" },   // A
  { "###", "#..", "###", "..#", "###" },   // S
  { "###", ".#.", ".#.", ".#.", ".#." },   // T
  { "##.", "#.#", "##.", "#.#", "#.#" },   // R
};
static int hwIdx(char ch) { for (int i = 0; i < 10; i++) if (HW_CH[i] == ch) return i; return -1; }
static int hwW(int g) { return g < 0 ? 3 : (int)strlen(HW[g][0]); }
static void stencilName(int cx, int cy) {
  const char* s = "MEDIEN HAMSTER";
  int w = -1; for (int i = 0; s[i]; i++) w += hwW(hwIdx(s[i])) + 1;
  int x = cx - w / 2;
  for (int i = 0; s[i]; i++) {
    int g = hwIdx(s[i]);
    if (g >= 0) for (int rr = 0; rr < 5; rr++) for (int k = 0; HW[g][rr][k]; k++) if (HW[g][rr][k] == '#') R(x + k, cy + 20 + rr, 1, 1, C_REFL);
    x += hwW(g) + 1;
  }
}
static void hamsterPole(int cx, int cy) {   // Pole-Dance: seitlich, Ruecken zur Stange, vorgebeugt, schoen pummelig
  int hp = gHip;
  uint16_t f = hcol(HC_FUR, false, false), fd = hcol(HC_FURD, false, false), pk = hcol(HC_PINK, false, false), pd = hcol(HC_PINKD, false, false), v = hcol(HC_VEST, false, false), vd = hcol(HC_VESTD, false, false), rf = hcol(HC_REFL, false, false);
  SE(cx + 12, cy - 14, 8, 13, fd); SE(cx + 10, cy + 1, 10, 4, pd);                       // hinteres Bein und Fuss
  SE(cx + 44 + hp, cy - 33 - hp, 3, 3, pd);                                                // Schwanz
  SE(cx + 27 + hp, cy - 34 - hp, 16, 16, fd); SE(cx + 26 + hp, cy - 35 - hp, 15, 15, f);   // dicker Po
  SE(cx + 19, cy - 17, 12, 16, f); SE(cx + 18, cy - 3, 8, 7, f); SE(cx + 15, cy + 2, 11, 4, pk);   // vorderes Bein
  SE(cx + 4, cy - 46, 20, 20, fd); SE(cx + 3, cy - 47, 19, 19, f);                         // runder Bauch
  SE(cx - 1, cy - 46, 15, 16, vd); SE(cx - 2, cy - 47, 15, 15, v);                          // Weste
  SEr(cx - 2, cy - 47, 15, 15, rf, -8, -6); SEr(cx - 2, cy - 47, 15, 15, rf, 2, 4);        // Reflexstreifen
  SE(cx - 14, cy - 30, 7, 11, f); SE(cx - 14, cy - 20, 6, 5, pk);                          // Arm, Pfote auf dem Knie
}
static void hamster(int cx, int cy, int mo, bool sk, bool dead, int32_t t, int an, bool full) {
  gcx = cx; gcy = cy;
  bool lying = mo == M_DEAD;                              // tot: liegt auf der Seite
  bool bored = sk && !dead && mo != M_SLEEP && !gParty;   // der Hamster wird nicht krank, ihm wird langweilig
  sk = false;
  if (gParty) { hamsterPole(cx, cy); gcx = cx - 28; gcy = cy + 14; }   // Pole-Dance: Koerper extra, hier nur der Kopf
  gRot = lying;
  for (unsigned i = 0; i < sizeof(HAM) / sizeof(HAM[0]); i++) {
    int dy = HAM[i][1]; if (gParty && dy > -54) continue; if (lying && dy >= 1) continue;   // Party: nur Kopf; tot: Case bleibt weg
    SR(HAM[i][0], dy, HAM[i][2], HAM[i][3], hcol(HAM[i][4], sk, dead));
  }
  if (full && !gParty && !lying) stencilName(cx, cy);   // Name als Schablonen-Aufdruck
  bool blink = (t % 3200) < 140; uint16_t E = C_EYE, fur = hcol(HC_FUR, sk, dead);
  if (mo == M_DEAD) { for (int i = 0; i < 8; i += 2) { SR(-13 + i, -70 + i, 2, 2, E); SR(-8 - i, -70 + i, 2, 2, E); SR(6 + i, -70 + i, 2, 2, E); SR(11 - i, -70 + i, 2, 2, E); } }
  else if (an == A_DIZZY) { for (int e = 0; e < 2; e++) { int o = e * 19; SR(-14 + o, -71, 9, 9, E); SR(-12 + o, -69, 5, 5, fur); SR(-11 + o, -68, 3, 3, E); } }
  else if (an == A_STARTLE) { SR(-14, -72, 9, 10, E); SR(5, -72, 9, 10, E); SR(-13, -71, 3, 3, C_WHITE); SR(6, -71, 3, 3, C_WHITE); }
  else if (bored && an == A_NONE) {   // gelangweilt: halb zu, der Blick wandert
    int lx = (int)((t / 900) % 3) - 1; uint16_t furd = hcol(HC_FURD, false, false);
    SR(-13, -70, 7, 8, E); SR(6, -70, 7, 8, E); SR(-11 + lx, -65, 2, 2, C_WHITE); SR(8 + lx, -65, 2, 2, C_WHITE);
    SR(-14, -71, 9, 5, fur); SR(5, -71, 9, 5, fur); SR(-14, -66, 9, 1, furd); SR(5, -66, 9, 1, furd);
  }
  else if (mo == M_HAPPY && !blink && (t / 1500) % 3 == 0) { SR(-14, -65, 3, 2, E); SR(-12, -67, 5, 2, E); SR(-8, -65, 3, 2, E); SR(5, -65, 3, 2, E); SR(7, -67, 5, 2, E); SR(11, -65, 3, 2, E); }   // Freudenaugen
  else if (mo == M_SLEEP || blink) { SR(-13, -66, 7, 2, E); SR(6, -66, 7, 2, E); }
  else { SR(-13, -70, 7, 8, E); SR(6, -70, 7, 8, E); SR(-12, -69, 3, 3, C_WHITE); SR(7, -69, 3, 3, C_WHITE); SR(-9, -65, 1, 1, C_WHITE); SR(10, -65, 1, 1, C_WHITE); }
  if (mo == M_SAD && !blink && an == A_NONE) SR(-11, -62, 2, 3, C_BUBBLE);
  uint16_t M = C_HMOUTH;
  if (an == A_STARTLE) SR(-2, -57, 4, 3, M);
  else if (bored && an == A_NONE) { if ((t % 6000) < 1300) { SR(-5, -58, 10, 7, M); SR(-3, -54, 6, 2, C_HPINK); } else SR(-3, -55, 6, 1, M); }   // gaehnt ab und zu
  else if (mo == M_HAPPY) { SR(-8, -58, 2, 2, M); SR(6, -58, 2, 2, M); SR(-6, -56, 12, 1, M); SR(-5, -55, 10, 3, M); SR(-2, -53, 4, 2, C_HPINK); SR(-21, -62, 7, 3, C_HPINK); SR(14, -62, 7, 3, C_HPINK); }   // strahlt
  else if (mo == M_OK) { SR(-5, -56, 4, 1, M); SR(1, -56, 4, 1, M); SR(-1, -57, 2, 1, M); }
  else if (mo == M_SAD) { SR(-4, -55, 8, 1, M); SR(-5, -54, 1, 1, M); SR(4, -54, 1, 1, M); }
  else if (mo == M_SICK) { SR(-4, -55, 3, 1, M); SR(-1, -56, 3, 1, M); SR(2, -55, 3, 1, M); }
  else if (mo == M_SLEEP) SR(-2, -56, 4, 1, M);
  else SR(-4, -55, 8, 1, M);
  gRot = false; gcx = cx; gcy = cy;
  if (!full) return;
  if (bored && an == A_NONE) { int n = (int)((t / 500) % 4); for (int i = 0; i < n; i++) SR(28 + i * 7, -96 - i * 3, 4, 4, C_TXT); }   // Denkblase "..."
  if (mo == M_HAPPY && an == A_NONE && !gParty) { int p = (int)((t / 40) % 100); if (p < 50) icon(I_HEART, cx + 28 + ((p / 10) % 2) * 3, cy - 98 - p / 2, 1, C_CHEEK); }
  if (mo == M_SLEEP && an != A_SLEEP) { bool z = ((t / 700) % 2) == 1; T(z ? "Z" : "z", cx + sc(26), cy + sc(-86) + (z ? 0 : 4), 2, C_TXT); }
}

static void drawPet(int sp, int cx, int cy, int mo, bool sk, bool dead, int32_t t, int an, int ap, bool full) {
  if (sp == 3) { hamster(cx, cy, mo, sk, dead, t, an, full); return; }
  gcx = cx; gcy = cy; gFlip = dead && full;   // tot: auf dem Ruecken
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
  else { SR(ex, ey, 4, 5, C_EYE); SR(ex + 1, ey + 1, mo == M_HAPPY ? 2 : 1, mo == M_HAPPY ? 2 : 1, C_WHITE); }
  if (mo == M_SAD && !blink && an == A_NONE) SR(ex + 1, ey + 6, 2, 3, C_BUBBLE);
  if (an == A_FEED && ap >= 600) { if ((ap / 150) % 2 == 0) SR(mx + 2, my - 1, ml - 1, 3, mc); else SR(mx, my, ml, 1, mc); }
  else if (an == A_STARTLE) SR(mx + 3, my - 1, ml < 5 ? ml : 5, 3, mc);
  else if (mo == M_HAPPY) { SR(mx, my, ml, 1, mc); SR(mx - 2, my - 1, 2, 1, mc); SR(mx + ml, my - 1, 2, 1, mc); SR(mx + 1, my + 1, ml - 2, 1, mc); SR(P.chx, P.chy, 7, 3, C_CHEEK); }   // breites Laecheln
  else if (mo == M_OK) SR(mx, my, ml, 1, mc);
  else if (mo == M_SAD) { SR(mx, my, ml, 1, mc); SR(mx - 2, my + 1, 2, 1, mc); }
  else if (mo == M_SICK) { SR(mx, my + 1, ml / 2, 1, mc); SR(mx + ml / 2, my, ml - ml / 2, 1, mc); }
  else if (mo == M_SLEEP) SR(mx, my, ml - 2, 1, mc);
  else SR(mx, my + 1, ml, 1, mc);
  gFlip = false;
  if (!full) return;
  if (sk) { SR(-46, -30, 2, 8, C_LOW); SR(-49, -27, 8, 2, C_LOW); }
  if (mo == M_HAPPY && an == A_NONE) { int p = (int)((t / 40) % 100); if (p < 50) icon(I_HEART, cx + sc(P.ex) + 8 + ((p / 10) % 2) * 3, cy + sc(P.ty) - 6 - p / 2, 1, C_CHEEK); }   // Herzchen steigen auf
  if (!S.sleeping && !dead) { int p = (t / 50) % 40; if (p < 12) R(cx + sc(P.hx) + ((p >> 2) % 2), cy + sc(P.hy) - p, 2, 2, C_BUBBLE); }
  if (mo == M_SLEEP && an != A_SLEEP) { bool z = ((t / 700) % 2) == 1; T(z ? "Z" : "z", cx + sc(34), cy + sc(-30) + (z ? 0 : 4), 2, C_TXT); }
}

static void drawHeader() {   // Hunger (Besteck), Spass (Controller), Energie (Blitz), darunter Gesundheit
  float vals[3] = { S.hunger, S.fun, S.energy };
  bool ham = S.species == 3;   // Medien Hamster: Kaffee statt Essen, Zigarette statt Spielen
  const int labs[3] = { ham ? I_CUP : I_FORK, ham ? I_CIG : I_PAD, I_BOLT };
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
  bool ham = S.species == 3;
  const int ids[5] = { ham ? I_CUP : I_FORK, ham ? I_CIG : I_PAD, S.sleeping ? I_SUN : I_MOON, ham ? I_BROOM : I_ROD, ham ? I_PARTY : I_KIT };
  const uint16_t cols[5] = { C_FOOD, C_PLAY, C_SEL, C_OK, C_CHEEK };
  for (int i = 0; i < 5; i++) {
    int x = 1 + i * 27, id = ids[i]; bool sel = S.menu == i;
    R(x, MENUY, 26, 22, sel ? C_SEL : C_PANEL);
    iconK(id, x + (26 - kW(id, 3, 2)) / 2, MENUY + (22 - kH(id, 3, 2)) / 2, 3, 2, sel ? C_BG : cols[i]);
  }
}

static void drawTrawl(int32_t t, int tap) {   // Schleppnetz kommt von rechts; oben Warnung und Tipp-Zaehler
  int ap = t - S.trawlAt, e = ap - TRAWL_MS; if (e > TRAWL_END) e = TRAWL_END;
  int nx = S.trawl == 1 ? 140 - ap * 72 / TRAWL_MS : 68 - e * 190 / TRAWL_END;
  for (int k = 0; k <= 84; k += 7) R(nx + k, 100, 1, 106, C_NET);
  for (int k = 0; k <= 106; k += 7) R(nx, 100 + k, 85, 1, C_NET);
  R(nx, 98, 85, 3, C_NETD); R(nx, 204, 85, 3, C_NETD);
  for (int k = 0; k < 4; k++) R(nx + 4 + k * 24, 93, 6, 6, C_LOW);                       // Schwimmer
  R(nx + 85, 99, 45, 1, C_NETD); R(nx + 85, 205, 45, 1, C_NETD);                          // Leinen zum Kutter
  if (S.trawl == 1) {
    R(0, 24, W, 11, ((t / 150) % 2) ? C_LOW : C_BG);
    const char* w = "NETZ! TIPPEN!"; T(w, (W - tw(w, 1)) / 2, 26, 1, C_TXT);
    for (int i = 0; i < TRAWL_NEED; i++) R(4 + i * 13, 38, 11, 5, i < tap ? C_OK : C_PANEL);
  }
}
static void drawAngel(int cx, int y, int32_t t) {   // Engelchen schwebt hinter dem toten Tier
  y += ((t / 400) % 2) * 2; int f = ((t / 300) % 2) * 2;
  R(cx - 30, y + 6 - f, 22, 5, C_WHITE); R(cx - 26, y + 11 - f, 18, 5, C_REFL); R(cx - 22, y + 16 - f, 14, 4, C_WHITE);   // Fluegel links
  R(cx + 8, y + 6 - f, 22, 5, C_WHITE); R(cx + 8, y + 11 - f, 18, 5, C_REFL); R(cx + 8, y + 16 - f, 14, 4, C_WHITE);      // Fluegel rechts
  R(cx - 8, y + 16, 16, 18, C_WHITE); R(cx - 11, y + 30, 22, 4, C_WHITE); R(cx - 3, y + 18, 6, 14, C_REFL);               // Gewand
  R(cx - 6, y + 4, 12, 12, C_SKINF); R(cx - 4, y + 8, 2, 2, C_EYE); R(cx + 2, y + 8, 2, 2, C_EYE); R(cx - 2, y + 12, 4, 1, C_LIPS);   // Kopf
  R(cx - 8, y - 4, 16, 2, C_GOLD); R(cx - 10, y - 2, 2, 3, C_GOLD); R(cx + 8, y - 2, 2, 3, C_GOLD); R(cx - 8, y, 16, 2, C_GOLD);   // Heiligenschein
}
static void drawGame(int32_t t) {
  int mo = mood();
  bool ham = S.species == 3;   // Medien Hamster: Messestand statt Wasser
  uint16_t bg = S.sleeping ? C_BGS : (isNight() ? C_BGN : C_BG);
  R(0, 0, W, H, bg);
  if (ham) drawMesse(S.sleeping);
  drawHeader();
  const int (*pl)[2] = ham ? PLH : PL;
  for (int i = 0; i < S.plastic; i++) { if (ham) drawMess(messAt(i), pl[i][0], pl[i][1], 6); else drawPlastic(i, pl[i][0], pl[i][1] + (int)(((t / 600) + i) % 2)); }
  // Animation der letzten Aktion (1,2 Sekunden, nur Optik)
  int alen = (ham && S.anim == A_MEDIC) ? 3000 : 1200;   // Party dauert laenger
  int ap = t - S.animStart, an = (S.anim && ap >= 0 && ap < alen) ? S.anim : A_NONE;
  if (an == A_FISH && ham) {   // aufraeumen
    int i = S.animArg % 8; cleanUp(S.animArg / 8, pl[i][0], pl[i][1], ap);
  } else if (an == A_FISH) {     // Angelschnur runter, Teil einhaken, hochziehen
    int i = S.animArg % 8, sx = pl[i][0], sy = pl[i][1], d = sy - 4 - WTOP;
    int ly = ap < 500 ? WTOP + d * ap / 500 : sy - 4 - d * (ap - 500) / 700;
    R(sx + 4, WTOP, 1, ly - WTOP + 3, C_TXT); R(sx + 1, ly + 3, 4, 1, C_TXT); R(sx + 1, ly + 1, 1, 2, C_TXT);
    drawPlastic(i, sx, ap < 500 ? sy : ly + 4);   // liegt noch, bis der Haken es greift
  } else if (an == A_WAVES && !ham) {     // Wellen ziehen durchs Wasser
    for (int r = 0; r < 2; r++) for (int k = 0; k < 16; k++) R((k * 16 + ap / 20) % 256 - 16, 40 + r * 110 + ((k + r) % 2) * 2, 8, 2, C_BUBBLE);
  }
  int jy = 0, dx = 0;
  if (an == A_PLAY) jy = ham ? 0 : -((ap * (1200 - ap)) / 24000);   // Sprung (Hamster raucht in Ruhe)
  else if (an == A_WAKE) jy = -((ap * (1200 - ap)) / 60000);         // kleiner Hopser
  else if (an == A_WAVES) jy = ham ? 0 : -(((ap / 150) % 2) * 3);    // schaukelt
  else if (an == A_MEDIC && ham) jy = -(((ap / 150) % 2) * 3);       // Hamster hueft auf der Party
  else if (an == A_STARTLE) jy = ap < 200 ? -(ap / 20) : (ap < 600 ? -10 : -((1200 - ap) / 60));
  if (an == A_DIZZY) dx = ham ? ((ap / 60) % 2) * 10 - 5 : ((ap / 60) % 2) * 4 - 2;   // wackelt
  else if (an == A_WAVES && ham) dx = ((ap / 120) % 2) * 6 - 3;                       // Hamster wackelt mit dem Case
  if (ham) { gkn = 1; gkd = 1; } else { gkn = 5; gkd = 4; }
  bool party = ham && an == A_MEDIC;   // Pole-Dance: Case weg, Hamster unten am Boden an der Stange
  int tz = 0, tdx = 0, tap = 0;        // Trawling: das Tier weicht nach oben aus bzw. wird erwischt
  bool trawl = S.trawl && !ham;
  if (trawl) {
    tap = S.trawlTaps > TRAWL_NEED + 2 ? TRAWL_NEED + 2 : S.trawlTaps;
    int e = t - S.trawlAt - TRAWL_MS; if (e > TRAWL_END) e = TRAWL_END;
    if (S.trawl == 1) tz = -tap * 4; else if (S.trawl == 2) tz = -((tap * 4 * (TRAWL_END - e)) / TRAWL_END); else tdx = ((e / 60) % 2) * 6 - 3;
  }
  int cx = DCX + dx + tdx, bob = (!ham && !S.sleeping && !S.dead && ((t / 500) % 2) == 1) ? 2 : 0, cy = (ham ? (party ? 203 : (S.dead ? HDCY - 34 : HDCY)) : DCY) + bob + jy + tz;
  bool pend = an == A_MEDIC && ap < 500 && !ham;   // Kapsel noch unterwegs: sieht noch krank aus
  if (pend && mo != M_SLEEP) mo = M_SICK;
  bool sk = (S.sick || pend) && !S.dead;
  if (party) { partyLights(ap); partyPole(cx); }
  if (ham && S.dead) R(cx - 52, cy + 36, 104, 3, C_DESKD);   // Schatten unter dem toten Hamster
  if (S.dead) drawAngel(DCX, ham ? 44 : 52, t);
  gParty = party; gHip = party ? ((ap / 150) % 2) * 3 : 0;
  drawPet(S.species, cx, cy, mo, sk, S.dead, t, an, ap, true);
  gParty = false; gHip = 0;
  if (trawl) drawTrawl(t, tap);
  const Spec& P = SPC[S.species];
  if (an == A_FEED) {
    if (ham) {   // Kaffee kommt angeflogen, dann wird geschluerft
      int tx = cx + sc(P.nx) - 4, ty = cy + sc(P.ny) - 4;
      if (ap < 600) mug(FOODX + (tx - FOODX) * ap / 600, FOODY + (ty - FOODY) * ap / 600, ap); else mug(tx, ty + (ap / 200) % 2, ap);
    } else if (ap < 600) {   // Wienerle oder Haferkeks kommt angeschwommen
      int f = S.animArg ? I_COOKIE : I_WIENER, tx = cx + sc(P.nx), ty = cy + sc(P.ny) - kH(f, 3, 2) / 2;
      iconK(f, FOODX + (tx - FOODX) * ap / 600, FOODY + (ty - FOODY) * ap / 600, 3, 2, S.animArg ? C_COOKIE : C_WIENER);
    }
  } else if (an == A_PLAY) {
    if (ham) {   // Zigarettenpause: Glut glimmt, Rauch steigt auf
      int mx = cx + 2, my = cy - 58;   // direkt am Mund, gross und deutlich
      R(mx, my, 8, 4, C_FILTER); R(mx + 8, my, 22, 4, C_WHITE); R(mx + 8, my + 3, 22, 1, C_REFL); R(mx + 7, my, 1, 4, C_LOW); R(mx + 30, my, 4, 4, ((ap / 150) % 2) ? C_EMBER : C_LOW);
      for (int i = 0; i < 4; i++) { int k = (ap / 8 + i * 12) % 40; R(mx + 31 + ((k / 6 + i) % 2) * 3, my - 5 - k, 3, 3, C_DESKD); }
    } else {     // Ball auf dem Kopf, Spritzer beim Eintauchen
      int bb = (ap % 400) * (400 - ap % 400) / 4000;
      iconK(I_BALL, cx + sc(P.bx) - 2, cy + sc(P.by) - kH(I_BALL, 3, 2) - bb, 3, 2, C_LOW);
      if (ap > 900) { int sp = (ap - 900) / 15; for (int i = 0; i < 5; i++) R(cx + sc(-34) + i * 20, cy + sc(16) + 6 - sp - (i % 2) * 4, 3, 3, C_BUBBLE); }
    }
  } else if (an == A_SLEEP) {  // Mond sinkt herab, Sterne funkeln
    int my = cy + sc(P.ty);
    iconK(I_MOON, cx + sc(20), ap < 600 ? -16 + (my + 16) * ap / 600 : my, 3, 2, C_SEL);
    for (int i = 0; i < 4; i++) if (((ap / 200) + i) % 2 == 0) { R(ST[i][0] - 2, ST[i][1], 5, 1, C_SEL); R(ST[i][0], ST[i][1] - 2, 1, 5, C_SEL); }
  } else if (an == A_WAKE) {   // Sonne geht auf
    iconK(I_SUN, cx + sc(26), cy + sc(P.ty + 14) - 20 * ap / 1200, 3, 2, C_SEL);
  } else if (an == A_MEDIC && ham) {   // Party
    partyFront(cx, cy, ap);
  } else if (an == A_MEDIC) {  // Kapsel faellt ins Maul, dann Funkeln
    if (ap < 500) { int px = cx + sc(P.bx + 2), pe = cy + sc(P.by - 4) - 5, py = WTOP + (pe - WTOP) * ap / 500; R(px, py, 5, 5, C_LOW); R(px + 5, py, 5, 5, C_TXT); }
    else for (int i = 0; i < 4; i++) { int x = cx + sc(-40 + i * 26), y = cy + sc(P.ty + 18) - (ap - 500) / 30 - (i % 2) * 8; R(x - 2, y, 5, 1, C_OK); R(x, y - 2, 1, 5, C_OK); }
  } else if (an == A_NEW) {    // Herzchen steigen auf
    for (int i = 0; i < 3; i++) iconK(I_HEART, cx + sc(-34 + i * 30), cy + sc(P.ty) - ap / 40 + (i % 2) * 6, 3, 2, C_CHEEK);
  } else if (an == A_DIZZY) {  // Sterne kreisen um den Kopf
    for (int i = 0; i < 3; i++) { int k = (ap / 100 + i * 3) % 8, x = cx + sc(P.ex - 6) + CIR[k][0] * 2, y = cy + sc(P.ty) - 6 + CIR[k][1] / 2; R(x - 2, y, 5, 1, C_SEL); R(x, y - 2, 1, 5, C_SEL); }
  } else if (an == A_STARTLE) {   // Ausrufezeichen
    int x = cx + sc(P.qx), y = cy + sc(P.ty + 10); R(x, y, 3, 10, C_LOW); R(x, y + 12, 3, 3, C_LOW);
  }
  // Meldung (Icon): unter dem Tier, beim Hamster rechts an der Wand
  int ix = ham ? 116 : INFX, iy = ham ? 99 : INFY;
  if (an == A_NONE && t < S.msgUntil && S.msgIcon >= 0) {
    int id = S.msgIcon, x = ix - icW(id), y = iy - icH(id);
    icon(id, x, y, 2, C_TXT);
    if (S.msgNo) iconNo(id, x, y, 2);
  }
  // Name und Alter rechts unten (der Hamster hat seinen Namen auf dem Case)
  if (!ham && !S.dead) {
    const char* name = NAMES[S.species];
    int nw = tw(name, 2) + 4;
    R(CORNX - nw + 1, NAMEY, nw - 2, 18, C_SEL); R(CORNX - nw, NAMEY + 1, nw, 16, C_SEL);
    T(name, CORNX - nw + 3, NAMEY + 2, 2, C_BG);
  }
  if (!S.dead) {
    char buf[12]; snprintf(buf, sizeof(buf), "%u", (unsigned)(S.age / 60));
    if (ham) { int ax = CORNX - tw(buf, 1) - 9; icon(I_CLOCK, ax, 204, 1, C_DIM); T(buf, ax + 9, 204, 1, C_DIM); }
    else {
      int cw = kW(I_CLOCK, 3, 2), ax = CORNX - tw(buf, 2) - cw - 2;
      iconK(I_CLOCK, ax, AGEY + 3, 3, 2, C_DIM); T(buf, ax + cw + 3, AGEY, 2, C_DIM);
    }
  }
  if (S.dead) {   // gestorben: Panel deckt alles dahinter ab, dann Text und Neustart
    R(0, 158, W, 82, C_BGS);
    const char* nm = NAMES[S.species]; int s2 = tw(nm, 2) <= 125 ? 2 : 1;
    T(nm, (W - tw(nm, s2)) / 2, s2 == 2 ? 164 : 168, s2, C_SEL);
    T("ist jetzt an einem", (W - tw("ist jetzt an einem", 1)) / 2, 184, 1, C_TXT);
    T("besseren Ort", (W - tw("besseren Ort", 1)) / 2, 194, 1, C_TXT);
    R(3, 210, 129, 24, C_SEL); T("Neues Spiel", (W - tw("Neues Spiel", 1)) / 2, 218, 1, C_BG);
  } else drawMenu(an, sk);
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
  const int ids[9] = { I_NEWG, I_FAST, I_SPK, I_WAVE, I_BATT, I_WIFI, I_DL, I_HELP, I_BACK };
  for (int r = 0; r < SET_N; r++) {
    int y = 4 + r * 24; bool sel = S.uiSel == r;
    uint16_t fg = sel ? C_BG : C_TXT;
    if (r == 8) { backRow(y, sel, 22); continue; }
    R(3, y, 129, 22, sel ? C_SEL : C_PANEL);
    if (r == 0) T("Neu", 8, y + 5, 2, fg);   // neues Spiel
    else iconK(ids[r], 8 + (20 - kW(ids[r], 3, 2)) / 2, y + (24 - kH(ids[r], 3, 2)) / 2, 3, 2, fg);
    if (r == 0) { if (S.species == 3) { gkn = 1; gkd = 6; drawPet(3, 104, y + 17, M_HAPPY, false, false, t, A_NONE, 0, false); } else { gkn = 1; gkd = 4; drawPet(S.species, 104, y + 14, M_HAPPY, false, false, t, A_NONE, 0, false); } }
    else if (r == 1) { const char* v = S.speed == 0 ? "1x" : (S.speed == 1 ? "10x" : "50x"); T(v, 128 - tw(v, 2), y + 4, 2, fg); }
    else if (r == 2 || r == 3) { int n = r == 2 ? S.vol : S.sens; if (n == 0) iconK(I_CROSSX, 110, y + 7, 3, 2, sel ? C_BG : C_LOW); else drawBars(105, y + 5, n, fg, sel ? C_FIN : C_DIM); }
    else if (r == 4) {
      char b[16]; if (S.batt < 0) strcpy(b, "--"); else snprintf(b, sizeof(b), "%d%%", S.batt);
      int x = 128 - tw(b, 2); T(b, x, y + 4, 2, fg);
      if (S.chg) iconK(I_BOLT, x - 13, y + 6, 3, 2, sel ? C_BG : C_SEL);
    }
    else if (r == 5) iconK(S.wifiOk ? I_CHECK : I_CROSSX, 112, y + 7, 3, 2, sel ? C_BG : (S.wifiOk ? C_OK : C_LOW));
    else if (r == 6) { char b[12]; snprintf(b, sizeof(b), "v%s", FW_NAME); T(b, 128 - tw(b, 2), y + 4, 2, fg); }
  }
  const char* cr = "(c) 2026 Daniel Lassak";
  T(cr, (W - tw(cr, 1)) / 2 + 1, 220, 1, S.uiSel == SET_N ? C_SEL : C_DIM);
  char vb[24]; snprintf(vb, sizeof(vb), "Version %s", FW_NAME); T(vb, (W - tw(vb, 1)) / 2 + 1, 230, 1, C_DIM);   // leuchtet, wenn der versteckte Platz gewaehlt ist
}
static void drawAkku(int32_t t) {   // Akku-Seite: Prozent, Verlauf der letzten 24 Stunden, Schlaf-Zaehler, Aus
  (void)t;
  R(0, 0, W, H, C_BGS);
  char b[16]; if (S.batt < 0) strcpy(b, "--"); else snprintf(b, sizeof(b), "%d%%", S.batt);
  int x = (W - tw(b, 3)) / 2; T(b, x, 12, 3, C_TXT);
  if (S.chg) iconK(I_BOLT, x - 20, 14, 3, 2, C_SEL);
  R(3, 46, 129, 78, C_PANEL);
  T("24h", 8, 50, 1, C_DIM); T("jetzt", 127 - tw("jetzt", 1), 50, 1, C_DIM);
  for (int i = 0; i < 24; i++) {
    int v = S.blog[i], bx = 8 + i * 5;
    if (v > 100) { R(bx, 116, 4, 1, C_DIM); continue; }
    int bh = v * 56 / 100; if (bh < 1) bh = 1;
    R(bx, 117 - bh, 4, bh, v >= 50 ? C_OK : (v >= 20 ? C_SEL : C_LOW));
  }
  char l[24];
  if (S.slpFail == 0) strcpy(l, "Schlaf ok"); else snprintf(l, sizeof(l), "Schlaf: %d Fehler", S.slpFail);
  T(l, 8, 130, 1, S.slpFail ? C_LOW : C_DIM);
  snprintf(l, sizeof(l), "Tiefschlaf: %dx", S.deepN); T(l, 8, 142, 1, C_DIM);
  R(3, 162, 129, 24, S.uiSel == 0 ? C_SEL : C_PANEL);
  T("Aus", INFX - tw("Aus", 2) / 2, 167, 2, S.uiSel == 0 ? C_BG : C_TXT);
  backRow(192, S.uiSel == 1, 24);
}
static void drawManual(int32_t t) {   // Kurzanleitung, drei Seiten (der Hamster kommt hier nicht vor)
  (void)t;
  R(0, 0, W, H, C_BGS);
  char b[24]; snprintf(b, sizeof(b), "ANLEITUNG %d/3", S.uiSel + 1);
  T(b, 6, 5, 1, C_SEL); R(3, 16, 129, 1, C_DIM);
  if (S.uiSel == 0) {
    const char* kk[4] = { "A", "B", "B+", "PWR" };
    const char* a1[4] = { "Ausfuehren", "Weiter", "Lang halten:", "Kurz druecken:" };
    const char* a2[4] = { "(Taste vorne)", "(Taste Seite)", "Menue", "Display aus" };
    for (int i = 0; i < 4; i++) {
      int y = 24 + i * 34;
      R(4, y, 38, 18, C_SEL); T(kk[i], 4 + (38 - tw(kk[i], 2)) / 2, y + 2, 2, C_BG);
      T(a1[i], 46, y + 1, 1, C_TXT); T(a2[i], 46, y + 10, 1, C_DIM);
    }
    T("Waehle dein Tier und", 6, 168, 1, C_TXT); T("kuemmere dich gut", 6, 178, 1, C_TXT); T("darum. Viel Spass!", 6, 188, 1, C_TXT);
  } else if (S.uiSel == 1) {
    const int ids[5] = { I_FORK, I_PAD, I_MOON, I_ROD, I_KIT };
    const uint16_t cols[5] = { C_FOOD, C_PLAY, C_SEL, C_OK, C_CHEEK };
    const char* a1[5] = { "Futter geben", "Spielen", "Schlafen legen", "Plastik angeln", "Medizin, wenn" };
    const char* a2[5] = { "gegen Hunger", "macht Spass", "nochmal = wecken", "Wasser sauber", "er krank ist" };
    for (int i = 0; i < 5; i++) {
      int y = 22 + i * 30;
      R(3, y, 129, 26, C_PANEL);
      iconK(ids[i], 8 + (20 - kW(ids[i], 3, 2)) / 2, y + (26 - kH(ids[i], 3, 2)) / 2, 3, 2, cols[i]);
      T(a1[i], 36, y + 4, 1, C_TXT); T(a2[i], 36, y + 14, 1, C_DIM);
    }
    T("Oben: Hunger, Spass,", 6, 178, 1, C_TXT); T("Energie, Gesundheit.", 6, 188, 1, C_TXT); T("Leere Balken = Hilfe!", 6, 198, 1, C_LOW);
  } else {
    const char* ln[12] = { "Schuetteln", "sanft = streicheln", "wild = Schwindel", "", "Trawling", "Netz naht! Schnell", "A tippen, dann weicht", "das Tier aus.", "", "Display aus", "Das Tier schlaeft,", "Zeit laeuft weiter." };
    for (int i = 0; i < 12; i++) T(ln[i], 6, 26 + i * 13, 1, (i == 0 || i == 4 || i == 9) ? C_SEL : C_TXT);
  }
  T(S.uiSel == 2 ? "A: Los geht's!" : "A weiter  B lang: zu", 6, 226, 1, C_DIM);
}
static void drawPicker(int32_t t) {   // Tier fuer das neue Spiel waehlen
  R(0, 0, W, H, C_BGS);
  for (int r = 0; r < 4; r++) {
    int y = 4 + r * 48; bool sel = S.uiSel == r;
    R(3, y, 129, 46, sel ? C_SEL : C_PANEL);
    if (r == 3) { gkn = 1; gkd = 4; drawPet(r, INFX, y + 26, M_HAPPY, false, false, t, A_NONE, 0, false); }
    else { gkn = 1; gkd = 2; drawPet(r, INFX, y + 18, M_HAPPY, false, false, t, A_NONE, 0, false); }
    T(NAMES[r], INFX - tw(NAMES[r], 1) / 2, y + 37, 1, sel ? C_BG : C_TXT);
  }
  if (S.first) T("Waehle dein Tier", (W - tw("Waehle dein Tier", 1)) / 2, 210, 1, C_TXT);   // erster Start / Neustart: kein Zurueck
  else backRow(4 + 4 * 48, S.uiSel == 4);
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
static void glove(int gx, int gy, uint16_t c, uint16_t acc) {   // Handschuh, Finger nach oben (gx/gy: Handflaeche links oben)
  R(gx, gy - 8, 3, 8, c); R(gx + 4, gy - 10, 3, 10, c); R(gx + 8, gy - 9, 3, 9, c); R(gx + 12, gy - 7, 3, 7, c);   // Finger
  R(gx, gy, 15, 12, c); R(gx - 5, gy + 3, 5, 3, c); R(gx - 4, gy + 1, 3, 3, c);                                    // Handflaeche, Daumen
  R(gx, gy + 4, 15, 2, acc); R(gx + 1, gy + 12, 13, 4, acc);                                                       // Knoechelpolster, Klettbund
}
static void drawBall(int32_t t) {   // Esmeraldas VT Orakel
  R(0, 0, W, H, C_BALLBG);
  const char* title = "Esmeraldas VT Orakel"; T(title, (W - tw(title, 1)) / 2, 4, 1, C_SEL);
  int ap = t - S.ballAt; bool think = S.ballAns >= 0 && ap >= 0 && ap < 2500;
  // Sprechblase
  R(4, 14, 127, 86, C_TXT); R(2, 16, 131, 82, C_TXT); R(62, 100, 12, 3, C_TXT); R(64, 103, 8, 2, C_TXT); R(66, 105, 4, 2, C_TXT);
  if (think) { int n = (ap / 250) % 3 + 1; for (int i = 0; i < n; i++) R(52 + i * 12, 54, 6, 6, C_BG); }
  else if (S.ballAns < 0 || ap >= 10500) T("?", 58, 47, 3, C_BG);   // Antwort verschwindet nach 8 s
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
  glove(103, 220, C_GLOVE2, C_SEL); glove(115, 222, C_GLOVE, C_VEST);   // Rigging-Handschuhe
  disc(x, 200, 15, think ? (((ap / 100) % 2) ? C_BALLWIN : C_ORB2) : C_ORB); R(x - 7, 192, 4, 4, C_WHITE);   // Kristallkugel
  if (think) for (int k = 1; k <= (ap / 150) % 3 + 1; k++) R(18 + k * 4, 204 - k * 2, 2, 4 + k * 4, C_SEL);   // Funkwellen
}
static void drawScene(int32_t t) {
  if (S.ui == 1) drawSettings(t);
  else if (S.ui == 2) drawPicker(t);
  else if (S.ui == 3) drawWifi(t);
  else if (S.ui == 4) drawUpdate(t);
  else if (S.ui == 5) drawBall(t);
  else if (S.ui == 6) drawAkku(t);
  else if (S.ui == 7) drawManual(t);
  else drawGame(t);
}

static void saveState() {
  prefs.begin("delfino", false);
  prefs.putFloat("h", S.hunger); prefs.putFloat("f", S.fun); prefs.putFloat("e", S.energy); prefs.putFloat("hp", S.health);
  prefs.putUInt("age", S.age); prefs.putUInt("seed", S.seed); prefs.putBool("sl", S.sleeping); prefs.putBool("dead", S.dead);
  prefs.putBool("sick", S.sick); prefs.putInt("pl", S.plastic); prefs.putInt("menu", S.menu); prefs.putUInt("now", S.now);
  prefs.putInt("sp", S.species); prefs.putInt("spd", S.speed); prefs.putInt("vol", S.vol); prefs.putInt("sens", S.sens); prefs.putUInt("mt", S.messT);
  prefs.putBytes("bl", S.blog, sizeof(S.blog)); prefs.putInt("sf", S.slpFail); prefs.putInt("dn", S.deepN);
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
    S.species = prefs.getInt("sp", 0); S.speed = prefs.getInt("spd", 0); S.vol = prefs.getInt("vol", 2); S.sens = prefs.getInt("sens", 2); S.messT = prefs.getUInt("mt", 0);
    if (prefs.isKey("bl")) prefs.getBytes("bl", S.blog, sizeof(S.blog));
    S.slpFail = prefs.getInt("sf", 0); S.deepN = prefs.getInt("dn", 0);
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
#include <esp_timer.h>
#include <esp_attr.h>
#include "kompat.h"
#include <sys/time.h>
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
static const uint32_t DEEP_MS = 600000;                     // Display so lange aus (10 min): Tiefschlaf, nur eine Taste weckt
static bool slpGpio = true;                                 // Leichtschlaf mit Tasten-Wecken; bei Fehlern nur noch mit Zeitgeber
static uint32_t bootMs = 0; static bool wokeDeep = false;
static uint32_t offWall = 0, offGame = 0, offBlock = 0, lastTrawl = 0; static bool offActive = false;   // Display aus: Spielstand eingefroren, Uhr laeuft weiter
static const uint32_t RTC_MAGIC = 0xDE1F0001;
RTC_DATA_ATTR static uint32_t rtcMagic = 0, rtcSec = 0, rtcLog = 0;   // ueberleben den Tiefschlaf
static uint32_t wallSec() { struct timeval tv; gettimeofday(&tv, nullptr); return (uint32_t)tv.tv_sec; }   // laeuft auch im Tiefschlaf weiter
static void logPush(uint8_t v) { memmove(S.blog, S.blog + 1, sizeof(S.blog) - 1); S.blog[sizeof(S.blog) - 1] = v; }
static void battLog() {   // jede Stunde den Akkustand merken; Luecken (Tiefschlaf) bleiben leer
  if (S.batt < 0) return;
  uint32_t ns = wallSec();
  if (rtcLog == 0 || ns < rtcLog) { rtcLog = ns; logPush((uint8_t)S.batt); return; }
  uint32_t hrs = (ns - rtcLog) / 3600; if (hrs == 0) return; if (hrs > 24) hrs = 24;
  for (uint32_t i = 1; i < hrs; i++) logPush(255);
  logPush((uint8_t)S.batt); rtcLog = ns;
}
// imuSleep() / extOff() stehen in kompat.h (Vorlagen mag der Arduino-Prototypengenerator nicht)
static void deepSleepNow() {   // Tiefschlaf: nur eine Taste weckt, die Uhr laeuft im Zeitgeber des Chips weiter
  S.deepN++; saveState();
  rtcMagic = RTC_MAGIC; rtcSec = offActive ? offWall : wallSec();   // Zeit seit Display-aus wird beim Aufwachen nachgeholt
  M5.Display.setBrightness(0); M5.Display.sleep();
  imuSleep(M5.Imu, 0); extOff(M5.Power, 0);
  uint64_t mask = (1ULL << BTN_A_PIN) | (idleA == idleB ? (1ULL << BTN_B_PIN) : 0);
  esp_sleep_enable_ext1_wakeup(mask, idleA ? ESP_EXT1_WAKEUP_ANY_LOW : ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_deep_sleep_start();
}

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
  if (!loadState()) { newEgg(esp_random()); S.ui = 2; S.first = 1; S.man = 1; }   // erster Start: erst ein Tier waehlen
  // Uhrzeit: kommt beim Kompilieren mit. Mit eingebauter Uhr laeuft sie auch weiter, wenn er aus ist.
  uint32_t bt = buildTime();
  bool hasRtc = M5.Rtc.isEnabled();
  if (hasRtc) {
    uint32_t clk = rtcNow();
    if (clk < bt) { clk = S.now > bt ? S.now : bt; rtcSet(clk); }   // Uhr leer (neu oder Akku ganz leer): ab letztem Stand weiter
    if (S.now == 0 || S.now > clk) S.now = clk; else catchUp(clk);  // Zeit, in der er aus war, nachholen
  } else {
    if (S.now < bt) S.now = bt;                                     // ohne Uhr steht die Zeit, solange er aus ist
    wokeDeep = esp_reset_reason() == ESP_RST_DEEPSLEEP && rtcMagic == RTC_MAGIC;
    if (wokeDeep) { uint32_t el = wallSec() - rtcSec; if (el > 0 && el < 8 * 86400u) catchUp(S.now + el); rtcMagic = 0; }   // Tiefschlaf-Zeit nachholen
  }
  S.wifiOk = wifiSaved();
  idleA = digitalRead(BTN_A_PIN); idleB = digitalRead(BTN_B_PIN);   // Ruhepegel der Tasten merken
  setCpuFrequencyMhz(80);                                            // reicht fuers Spiel und spart Strom
  Serial.printf("Delfino %s (Build %d): eingebaute Uhr %s, Spielzeit %02d:%02d, Startgrund %d\n", FW_NAME, FW_VERSION, hasRtc ? "gefunden" : "nicht gefunden", hourOf(S.now), (int)((S.now / 60) % 60), (int)esp_reset_reason());
  saveState();
  M5.Mic.end(); M5.Speaker.end();                 // Mikro wird nicht gebraucht, Lautsprecher nur fuer Melodien
  lastTick = lastDraw = lastSave = lastAct = lastImu = shWin = lastAlert = millis(); lastBatt = 0; bootMs = lastAct;
}

void loop() {
  M5.update();
  uint32_t now = millis(); int32_t t = (int32_t)now;
  // Tasten: A (vorne) = ausfuehren, B (Seite) kurz = weiter, B lang = Einstellungen
  bool a = M5.BtnA.wasPressed(), bl = M5.BtnB.wasHold(), bn = !bl && M5.BtnB.wasClicked();
  if (wokeDeep && now - bootMs < 800) { a = bl = bn = false; }   // die weckende Taste zaehlt nicht als Aktion
  if (now < offBlock) { a = bl = bn = false; }                    // nach 'Display aus' per Power-Taste: 2,5 s lang keine Tasten
  if ((a || bl || bn) && !screenOn) { a = bl = bn = false; lastAct = now; }   // Display war aus: erster Druck weckt nur
  bool any = false;
  if (a) { pressOk(t); any = true; saveState(); }
  if (bl) { pressLong(); any = true; }
  else if (bn) { pressNext(); any = true; }
  if (any) { lastAct = now; if (!S.snd) sfx(t, S_CLICK); }
  if (M5.BtnPWR.wasClicked() && screenOn && S.ui < 3) { lastAct = now - OFF_MS - 1; offBlock = now + 2500; }   // Ausschalttaste kurz: Display aus, Stromsparschlaf
  // Auftraege aus den Einstellungen
  if (S.req == 1) { S.req = 0; apStart(); }
  if (apOn) {
    dns.processNextRequest(); web.handleClient();
    uint32_t m = millis();   // frisch messen: apSince/apSaved koennen juenger sein als 'now'
    if ((apSaved && m - apSaved > 5000) || m - apSince > AP_MAX_MS) S.ui = 1;   // gespeichert oder zu lange offen: zu
    if (S.ui != 3) apStop();
  }
  if (S.req == 2) { S.req = 0; doUpdate(); lastTick = millis(); }
  if (S.req == 3) { S.req = 0; saveState(); M5.Display.setBrightness(0); M5.Display.sleep(); M5.Power.powerOff(); delay(500); deepSleepNow(); }   // ganz aus (haengt er am Kabel: Tiefschlaf)
  // Zeit (mit Zeitraffer): jede Sekunde ein Spielschritt, die Uhr laeuft mit
  uint32_t tickMs = TICK_MS[S.speed];
  if (!screenOn) lastTick = now;   // Display aus: das Spiel steht, nur die Uhr laeuft; beim Aufwachen wird die Zeit nachgeholt
  else {
    int n = 0;
    while (now - lastTick >= tickMs && n < 500) { gStep(); S.now++; lastTick += tickMs; n++; }
    if (now - lastTick >= tickMs) lastTick = now;
  }
  trawlTick(t);   // Trawling: nur Wassertiere, nur wenn man gerade im Spiel ist
  if (screenOn && S.ui == 0 && now - lastAct < 20000 && now - lastTrawl >= 1000) { lastTrawl = now; if (rnd() % 240 == 0) { trawlStart(t); if (S.trawl) lastAct = now; } }
  // Akku alle 5 s abfragen
  if (now - lastBatt >= 5000 || lastBatt == 0) { lastBatt = now; S.batt = M5.Power.getBatteryLevel(); S.chg = (int)M5.Power.isCharging() == 1; battLog(); }
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
    if (screenOn) { M5.Display.setBrightness(0); M5.Display.sleep(); screenOn = false; curBright = 0; offWall = wallSec(); offGame = S.now; offActive = true; saveState(); }
  } else {
    if (!screenOn) {
      M5.Display.wakeup(); screenOn = true; curBright = -1; lastDraw = 0;
      if (offActive) { offActive = false; catchUp(offGame + (wallSec() - offWall)); lastTick = now; lastSave = now; }   // vergangene Zeit nachholen
    }
    bool moving = S.anim && (int32_t)(now - S.animStart) < 1200;
    if (now - lastDraw >= (moving ? 40u : 100u)) { drawScene(t); cv.pushSprite(0, 0); lastDraw = now; }
    int target = (!keepOn && idle > DIM_MS) ? (S.sleeping ? 5 : BRIGHT_DIM) : BRIGHT_ON;
    if (target != curBright) { M5.Display.setBrightness(target); curBright = target; }
  }
  if (now - lastSave >= (screenOn ? 30000u : 600000u)) { saveState(); lastSave = now; }   // bei aus-Display selten: spart Strom und Flash
  if (screenOn) { delay(4); return; }   // kurze Pause: der Prozessor darf zwischendurch ruhen
#if LEICHTSCHLAF
  // Display aus: Prozessor schlaeft, wacht alle bis zu 30 s fuer die Spielzeit auf oder sofort bei Tastendruck.
  // Nach 10 min ohne Bedienung: Tiefschlaf. Der Sketch prueft, ob der Schlaf wirklich klappt.
  if (playId < 0 && !apOn) {
    if (M5.BtnA.isPressed() || M5.BtnB.isPressed()) { delay(20); return; }
    idleA = digitalRead(BTN_A_PIN); idleB = digitalRead(BTN_B_PIN);   // Ruhepegel jetzt lesen, solange keine Taste gedrueckt ist
    if (!keepOn && idle > DEEP_MS) deepSleepNow();
    uint32_t wakeMs = TICK_MS[S.speed] * 400UL; if (wakeMs > 30000) wakeMs = 30000;
    if (!slpGpio) wakeMs = 150;
    esp_sleep_enable_timer_wakeup((uint64_t)wakeMs * 1000ULL);
    if (slpGpio) {
      gpio_wakeup_enable((gpio_num_t)BTN_A_PIN, idleA ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
      gpio_wakeup_enable((gpio_num_t)BTN_B_PIN, idleB ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
      esp_sleep_enable_gpio_wakeup();
    }
    int64_t t0 = esp_timer_get_time();
    esp_err_t er = esp_light_sleep_start();
    int64_t dt = esp_timer_get_time() - t0;
    bool glitch = slpGpio && esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_GPIO && dt < 20000 && digitalRead(BTN_A_PIN) == idleA && digitalRead(BTN_B_PIN) == idleB;
    if (er != ESP_OK || glitch) {   // Schlaf abgelehnt oder sofort wieder wach: zaehlen, danach nur noch mit Zeitgeber schlafen
      if (S.slpFail < 99) S.slpFail++;
      if (slpGpio) { slpGpio = false; esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_GPIO); }
      if (er != ESP_OK) delay(20);
    }
  } else delay(20);
#else
  delay(20);
#endif
}
#endif
