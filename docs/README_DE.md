# nice!60 – Regionales reaktives RGB / Energieverwaltung

## Status: QUELLCODE-VORBEREITUNG, NICHT AUF HARDWARE VALIDIERT

Dieses ZIP enthält editierte ZMK-Konfigurationen und ein experimentelles Zephyr-Modul. **Ein erfolgreicher Firmware-Build wurde für diese neue Codefassung noch nicht bestätigt.** Keinesfalls `settings_reset` aufspielen.

## Vorgeschichte und gesicherte Befunde

1. DIY nice!60 / nRF52840, ZMK und Bluetooth erfolgreich konfiguriert; zwei Layer inklusive WASD-Pfeilen, Home/End, Page Up/Down, F1–F12.
2. Dauerlaufende RGB-Animation entlädt den Akku; Akkukapazität laut Nutzer deutlich höher als normal.
3. Backup-Branch `backup-before-reactive-rgb` erstellt; Entwicklungsbranch `feature/reactive-rgb` erstellt.
4. Hersteller-Schaltplan zeigt zwölf WS2812B, D64–D75, seriell verschaltet. Die von der Tastatur heruntergeladenen Build-Logs zeigen `chain-length = <0xc>` (hex. 12), `compatible = "worldsemi,ws2812-spi"` und `zmk,underglow = &led_strip`.
5. Foto der Vorder-/Rückseite der nice!60 wurde bereitgestellt. Zwölf weiße LED-Bauteile sichtbar. **Ihre Zuordnung zu den SPI-Kettenindizes ist weiterhin ungeprüft.**
6. `CONFIG_ZMK_IDLE_TIMEOUT=30000`, `CONFIG_ZMK_SLEEP=y`, `CONFIG_ZMK_IDLE_SLEEP_TIMEOUT=900000` waren bereits im Entwicklungsbranch gesetzt; GitHub Actions hatte diese Konfiguration erfolgreich gebaut.
7. Die früheren einzelnen Modul-Commits bauten zunächst grün, obwohl `src/reactive_rgb.c` fehlte. Vermutlich wurde `NICE60_REACTIVE_RGB` wegen `depends on ZMK` (kein solches globales Symbol in eingesehenem ZMK-Kconfig) deaktiviert. Die Abhängigkeit wurde hier entfernt.

## Neue Implementierung

- `src/reactive_rgb.c` abonniert `zmk_position_state_changed`, reagiert nur auf `state=true` (Drücken), schreibt eine kurze grün-türkise Farbe auf den WS2812-LED-Strip und schaltet die betroffenen Bereiche nach 1000 ms wieder aus.
- `src/led_regions.h` enthält eine **vorläufige** Zuordnung von 61 Tastaturpositionen auf 12 LEDs. Die physische Lage ist bekannt, ihre elektrische Indexreihenfolge aber nicht. Vor einer endgültigen Nutzung muss die Zuordnung anhand des PCB-Layouts oder eines geeigneten LED-Indextests überprüft werden.
- Alte RGB-Fn-Kommandos wurden **auskommentiert und daneben durch `&trans` ersetzt**, damit der deaktivierte ZMK-Animationscontroller nicht auf denselben Strip zugreift. Alle anderen Belegungen bleiben unverändert.
- `zephyr/Kconfig` entfernt die problematische `depends on ZMK`-Bedingung und wählt `LED_STRIP`; `zephyr/module.yml` und `zephyr/CMakeLists.txt` binden das Modul ein.

## Was noch zu tun ist

1. Inhalt dieses Projekt-Roots statt des bisherigen Entwicklungsstands auf `feature/reactive-rgb` nach GitHub hochladen (z.B. über `git clone` + `rsync` + `git commit` + `git push`). Den `.github`-Workflow beibehalten.
2. GitHub Actions auf **grünen Build mit aktiver** `CONFIG_NICE60_REACTIVE_RGB=y` und tatsächlich kompiliertem `reactive_rgb.c` prüfen; bei Fehlern anhand des konkreten Logs nachbessern.
3. Firmware **erst nach bestandenem Build** flashen; nur `nice60_zmk`-UF2, nicht `settings_reset`.
4. Taste links/mitte/rechts drücken; regionale LED-Indizes verifizieren. Korrigieren in `src/led_regions.h`, neu bauen.
5. Bei ausgeschalteten LEDs in Standby und nach 15 Minuten Stromaufnahme/Akkulaufzeit prüfen.

## Wesentlicher Vorbehalt zum Akku

WS2812B-LEDs brauchen auch dann einen Ruhestrom, wenn ihre Helligkeit auf null gesetzt ist. **Softwareseitig schwarz bedeutet nicht zwingend stromlos.** Das originale Hardware-Schaltbild nennt einen `Manual RGB Switch`; je nach Platinenrevision lässt sich die LED-Stromversorgung separat abschalten. Für monatelange Akkulaufzeit kann eine tatsächliche Versorgungstrennung nötig sein. Ohne Strommessung ist die Laufzeit nicht garantiert.

## Quellen

- https://github.com/kaipoeritz/nice60-zmk-config
- https://nicekeyboards.com/docs/nice-60/
- https://nicekeyboards.com/docs/nice-60/schematic/
- https://zmk.dev/docs/development/module-creation
- https://docs.zephyrproject.org/latest/develop/modules.html
- https://github.com/zmkfirmware/zmk/blob/main/app/src/rgb_underglow.c
- https://github.com/zmkfirmware/zmk/blob/main/app/include/zmk/events/position_state_changed.h

## Bilder

- `images/nice60_original_pcb.jpg`: Herstellerbild, aus der aktuellen Unterhaltung bereitgestellt.
- `images/nice60_rgb_schematic_screenshot.jpg`: RGB-Schaltplanausschnitt aus der aktuellen Unterhaltung.

Dokumentationsstand: 2026-10-14. Die Firmware ist ein **Entwicklungsartefakt**, kein geprüftes Release.
