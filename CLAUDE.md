# C64-IRC-Server (Lernprojekt)

Diese Datei ist die Übergabe aus einer längeren Planungsunterhaltung. Sie beschreibt das Projekt, die Arbeitsregeln und den Stand.

## Worum es geht

Ein IRC-Server, dessen Logik auf einem echten C64 läuft. Das Netzwerk übernimmt eine TeensyROM-Cartridge. Matthias überlegt, das bis zum 40C3 (Ende Dezember 2026) zu bauen und eventuell mitzubringen. Entschieden ist das noch nicht.

Das eigentliche Ziel ist Lernen: Matthias will C, C++ und 6502-Assembler von Hand schreiben lernen.

## Arbeitsregeln für Claude

Diese Regeln haben Vorrang vor dem üblichen Verhalten als Coding-Agent.

1. **Matthias schreibt den Projektcode selbst.** Liefere keinen fertigen Projektcode. Schreibe keine Lösungen für Übungen, keine ganzen Funktionen und ändere keine Projektdateien, außer er bittet ausdrücklich darum.
2. **Hinweise in Stufen.** Erst eine Frage oder eine Fundstelle, dann ein engerer Tipp. Die Lösung gibt es nur, wenn er sie ausdrücklich verlangt.
3. **Review statt Umschreiben.** Sag, was falsch oder riskant ist und warum. Schreib den Code nicht neu.
4. **Kurze Beispiele sind erlaubt**, wenn sie ein Sprachkonzept erklären und nicht die aktuelle Aufgabe lösen.
5. **Werkzeug-Hürden abkürzen.** Build-Probleme, Installation, Editor-Konfiguration und Hilfsskripte außerhalb des Projektkerns darfst du direkt lösen. Daran lernt man nichts.
6. **Editor-Funktionen bleiben an**, auch Copilot. Schlag nicht vor, sie abzuschalten. Das Thema ist geklärt.
7. **Ehrlich einschätzen.** Der Zeitplan ist knapp. Sag es, wenn etwas unrealistisch wird.

## Stil

- Antworte auf Deutsch, Code und Kommentare im Code auf Englisch.
- Verwende keine Gedankenstriche (Em-Dashes).
- Matthias kann etwas Python. Erkläre C als Unterschied zu Python.
- Er hat noch nie richtig C, C++ oder 6502-Assembler geschrieben.
- Geh Schritt für Schritt vor: ein Schritt pro Antwort, dann auf seine Rückmeldung warten.
- Erkläre, was ein Befehl, eine Datei oder eine Einstellung bewirkt, statt sie nur zu nennen.
- Er legt Dateien gern selbst im Terminal an. Gib den Inhalt an, nicht einen fertigen Einzeiler.
- Kleine Commits, einer pro abgeschlossener Änderung. Vor dem Commit `git status` und `git diff` ansehen.

## Architektur

- **TeensyROM-Firmware:** ein eigener IO-Handler mit Listener auf Port 6667. Er nimmt mehrere Clients an und reicht ganze Zeilen mit Client-Nummer an den C64 weiter. Kein Raspberry Pi, kein externer Multiplexer.
- **C64:** der eigentliche ircd. Er verwaltet Nicks und Channels, parst jede Zeile, setzt Regeln durch und entscheidet, wer welche Nachricht bekommt.
- **Schnittstelle (Entwurf, nicht final):**

  | Adresse | Zweck |
  |---|---|
  | `$DE00` | Status: Zeile wartet, Senden frei |
  | `$DE01` | Client-Nummer (Absender bzw. Empfänger) |
  | `$DE02` | Daten, Byte für Byte |
  | `$DE03` | Kommando: Zeile quittieren, abschicken, Client trennen |

- **IRC-Umfang:** Minimal-Subset von RFC 1459 mit NICK, USER, PING/PONG, JOIN, PART, PRIVMSG, QUIT und den nötigen Numerics. Eine Zeile ist höchstens 512 Bytes lang.

## Lernplan

1. **Phase 1: IRC-Kern in C auf dem Rechner.** Reine Logik: Zeile rein, Antworten raus, ohne Netzwerk und ohne C64. Übersetzt mit `gcc`.
2. **Phase 2: derselbe Code auf dem C64.** Übersetzt mit cc65, erst im Emulator VICE, dann auf der echten Maschine. Hier kommen die ersten Assembler-Routinen (ca65) für den Registerzugriff dazu.
3. **Phase 3: der Firmware-Handler** in einem Fork der TeensyROM-Firmware (C++).

Wird Phase 3 bis zum Congress nicht fertig, darf Claude den Handler ausnahmsweise liefern, und Matthias schreibt ihn danach selbst neu. Das entscheidet er dann.

## Stand am 15.11.2026

- **System:** Arch Linux mit KDE, Shell zsh.
- **Projektordner:** `~/Projects/c64ircd`, Repository `github.com/mschabhuettl/c64ircd` (öffentlich, MIT-Lizenz).
- **Installiert:** `gcc`, `gdb`, `make`, `clang`, `cc65-git` (2.19.r1706), VICE.
- **Editor:** VS Code (`code` mit `code-marketplace` und `code-features`), Erweiterungen clangd, Error Lens, CodeLLDB, VS64, Hex Editor, Claude Code.
- **Im Repository vorhanden:** `.gitignore` (C-Vorlage plus C64- und Werkzeug-Einträge), `compile_flags.txt` (`-Wall`, `-Wextra`), `.clang-format` (LLVM, 4 Leerzeichen). Der Ordner `build/` nimmt übersetzte Programme auf und wird von Git ignoriert.
- **Noch offen:** Welche Kartenvariante vorliegt (TeensyROM oder TeensyROM+), ist nicht geklärt.

### Nächste Schritte

1. Programm 1: gibt "hallo" aus.
2. Dasselbe Programm für den C64 übersetzen und in VICE starten.
3. Programm 2: liest eine Zeile von der Tastatur und gibt nur das erste Wort aus. Aus `JOIN #40c3` wird `JOIN`. Das ist der erste Baustein des Parsers.

### Befehle

```
gcc -Wall -Wextra -g -fsanitize=address,undefined -o build/hallo hallo.c
./build/hallo

cl65 -t c64 -o build/hallo.prg hallo.c
x64sc build/hallo.prg
```

## Lernmaterial

- Beej's Guide to C Programming: https://beej.us/guide/bgc/
- Easy 6502 (Assembler im Browser): https://skilldrick.github.io/easy6502/
- cc65-Dokumentation: https://cc65.github.io/doc

## Notizen zur Firmware (für Phase 3)

- Repository: https://github.com/SensoriumEmbedded/TeensyROM
- Bauen: Node.js, `npm run build:tr` oder `npm run build:tr-plus`, Flashen per USB.
- Handler liegen in `Source/Teensy/MinimalBoot/Common/IO_Handlers/`, einer pro Datei. Registriert werden sie in `IOHandlers.h` (Array `IOHandler[]`) und `Menu_Regs.h` (`enumIOHandlers`, `LastSelectableIOH`).
- Kleinstes Beispiel zum Lesen: `IOH_GeoRAM.c`. Die SwiftLink-Emulation (`IOH_Swiftlink.c`) bleibt unangetastet.
- Vorlage für einen Listener: `EthernetServer tcpServer(2112)` in `Teensy.ino`.
- Der Registerzugriff (`IO1Hndlr`) läuft im Bus-Takt des C64 und darf nicht blockieren. Alles Netzwerk gehört in den `PollingHndlr`.
- NativeEthernet hat standardmäßig 8 Sockets (`setSocketNum()` erhöht das) und blockiert beim Schreiben, wenn der Sendepuffer voll ist.
- Am Congress ist mit absichtlich kaputten Eingaben zu rechnen: überlange Zeilen, Flooding, ungültige Befehle.
