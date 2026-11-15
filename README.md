# Delfino

Ein kleines Tamagotchi fuer den **M5StickS3**: Delfin, Orca oder Schildkroete im Wasser, oder der Medien Hamster am Messestand.
Fuettern (vegane Wienerle und Haferkekse), spielen, schlafen legen, Plastik rausfischen,
Medizin geben. Schuetteln macht Wellen.

## Bedienung

- **Taste A (vorne):** ausfuehren
- **Taste B (Seite):** weiter
- **Taste B lang:** Einstellungen (neues Spiel mit Tierwahl, Zeitraffer 1x/10x/50x, Lautstaerke,
  Sensoren, Akku, WLAN, Update)

## Updates ohne Laptop

1. Einmalig per Laptop flashen (Arduino IDE, Board *M5StickS3*, Bibliothek *M5Unified*).
2. Am Stick: **B lang -> WLAN**. Am iPhone ins WLAN **Delfino-Setup** wechseln und
   dort das eigene WLAN eintragen.
3. Jede Aenderung an `delfino/delfino.ino` auf `main` baut GitHub automatisch
   (siehe *Actions*) und legt ein Release an.
4. Am Stick: **B lang -> Update**. Er laedt die neueste Version und startet neu.

Das Repo muss **oeffentlich** sein, damit der Stick die Releases ohne Passwort laden kann.
Im Code stehen keine Zugangsdaten; das WLAN-Passwort liegt nur auf dem Stick.

## Spielen auf dem iPhone (ohne Stick)

Unter `docs/` liegt eine Handy-Version mit denselben Tieren und derselben Logik.
Sie laeuft ueber GitHub Pages: https://elitepole92.github.io/delfino/
Schuetteln geht ueber den Bewegungssensor des iPhones (einmal "Schuetteln einschalten"
antippen und erlauben). In Safari ueber Teilen -> "Zum Home-Bildschirm" wird sie zur App.
Der Spielstand liegt nur im Browser des jeweiligen Handys.
