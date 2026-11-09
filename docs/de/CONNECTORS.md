# ES3C35P / 3,5-Zoll-ESP32-S3 – Externe Anschlüsse und Pinbelegung

Diese Dokumentation beschreibt die externen Anschlüsse der Platine anhand des **Originalschaltplans des Herstellers** (`ESP32-S3原理图.pdf`, Blatt 1) und der **ESP32-S3-Pinbelegungstabelle von LCDWiki** (`Eingefügtes Markdown(3).md`, aus LCDWiki übernommen). Aufgeführt werden **elektrische Netzbezeichnungen und Pin-Nummern aus dem Schaltplan**. Die tatsächliche Blickrichtung auf die Platine sowie Steckertyp und Rastermaß sind damit noch nicht überprüft.

> **Sicherheit:** Vor dem Verdrahten die Stromversorgung trennen. **P2 führt an Pin 1 +5 V; P3 und P4 führen an Pin 1 dagegen 3,3 V.** Die Pin-Nummerierung darf nicht ohne Prüfung als Reihenfolge von links nach rechts auf der Platinenrückseite interpretiert werden. Pin-1-Markierung und Ausrichtung am echten Board kontrollieren. Niemals 5 V direkt an ESP32-S3-GPIOs anlegen.

## 1. Vierpolige externe Steckverbinder

| Anschluss | Schaltplan-Kennung | Pin | Netz / Platinenbezeichnung | ESP32-S3-Signal | Beschreibung | Versorgung / Logik |
|---|---|---:|---|---|---|---|
| UART | **P2** | 1 | `+5` | – | 5-V-Versorgungsschiene | **5 V** |
| UART | **P2** | 2 | `GND` | – | Masse | 0 V |
| UART | **P2** | 3 | `TXD0` | UART0 TX | Serielle Sendeleitung (über 100-Ω-Serienwiderstand) | 3,3-V-Logik |
| UART | **P2** | 4 | `RXD0` | UART0 RX | Serielle Empfangsleitung (über 100-Ω-Serienwiderstand) | 3,3-V-Logik |
| Erweiterungs-GPIO | **P3** | 1 | `VCC3V3` | – | 3,3-V-Versorgungsschiene | **3,3 V** |
| Erweiterungs-GPIO | **P3** | 2 | `GND` | – | Masse | 0 V |
| Erweiterungs-GPIO | **P3** | 3 | `IO46` | GPIO46 | Erweiterungssignal; Einschränkungen beim Booten | 3,3-V-Logik |
| Erweiterungs-GPIO | **P3** | 4 | `IO45` | GPIO45 | Erweiterungssignal; Einschränkungen beim Booten | 3,3-V-Logik |
| I²C | **P4** | 1 | `VCC3V3` | – | 3,3-V-Versorgungsschiene | **3,3 V** |
| I²C | **P4** | 2 | `GND` | – | Masse | 0 V |
| I²C | **P4** | 3 | `IIC_SCL` | GPIO39 | Gemeinsame I²C-Taktleitung | 3,3-V-Logik |
| I²C | **P4** | 4 | `IIC_SDA` | GPIO38 | Gemeinsame I²C-Datenleitung | 3,3-V-Logik |

**Quelle:** `ESP32-S3原理图.pdf`, Blatt 1, unten links: Abschnitt `外部接口` (externe Anschlüsse) mit P2 (`串口`, seriell), P3 (`扩展IO`, Erweiterungs-I/O) und P4 (`I2C`). Die LCDWiki-Pintabelle beschreibt zusätzlich GPIO38/39 und GPIO45/46.

### Hinweis zur UART-GPIO-Zuordnung

Die Hersteller-Pintabelle bezeichnet `RXD0(IO43)` und `TXD0(IO44)`. Eine unabhängige ES3C35P-Hardwarereferenz beschreibt die UART-Richtungen abweichend. **Der Schaltplan benennt die Steckerleitungen eindeutig als `TXD0` und `RXD0`; die Zuordnung dieser Leitungen zu den GPIO-Nummern sollte jedoch für die jeweilige Platinenrevision geprüft werden, bevor sie als praktisch bestätigt gilt.** Bei der Verdrahtung die Netzbezeichnungen verwenden: RX des externen Geräts an `TXD0`, TX an `RXD0`, gemeinsame Masse und 3,3-V-Logikpegel.

## 2. Weitere Anschlüsse und Schaltungsteile

| Schaltplan-Kennung | Netz(e) / Bauteil | Funktion | Hinweise |
|---|---|---|---|
| **JP1** | `BAT+`, `GND` | Externer Akkuanschluss | Bereich für Akku-Laden und -Entladen; Polarität am physischen Anschluss vor Nutzung prüfen |
| **U2** | `TP4054_C668215` | Ladeschaltung für eine Lithium-Zelle | Ein vorhandener Ladecontroller bestätigt **nicht** automatisch, dass ein bestimmter Akku, dessen Schutzschaltung oder der Ladestrom geeignet ist |
| **GPIO8** | `BAT_ADC` | Messung der Akkuspannung | Spannungsteiler R14/R15 mit jeweils 100 kΩ; am ADC liegt nominell etwa die halbe Akkuspannung an |
| **JP3** | `SP+`, `SP-` | Externer Lautsprecheranschluss | Differenzieller Ausgang des SC8002B-Verstärkers; **keiner der Lautsprecheranschlüsse darf als GND angenommen werden** |
| **U5** | `ES8311` | Audiocodec | I²S-Audioschnittstelle und gemeinsamer I²C-Steuerbus |
| **U6** | `SC8002B` | Lautsprecherverstärker | `AUDIO_EN` an GPIO1; laut Pintabelle aktiv LOW |
| **SD_CARD1** | `SD_CLK`, `SD_CMD`, `SD_D0`–`SD_D3` | microSD-Steckplatz | GPIO5, GPIO4 beziehungsweise GPIO6/7/2/3 |
| **USB2** | USB-C, `VBUS`, `USB+`, `USB-` | USB-Versorgung und Daten | Die USB-Versorgung ist nicht mit der 3,3-V-GPIO-Versorgung gleichzusetzen |

**Quelle:** `ESP32-S3原理图.pdf`, Blatt 1: `电池充放电电路` (Akku-Lade-/Entladeschaltung), `音频` (Audio), `TF卡` (microSD) und `USB接口` (USB).

## 3. Gemeinsam genutzte Busse und bereits belegte GPIOs

| Funktion | GPIOs | Bedeutung für Erweiterungen |
|---|---|---|
| Gemeinsamer I²C-Bus | GPIO38 SDA, GPIO39 SCL | Touchcontroller und ES8311 nutzen denselben Bus wie P4; zusätzliche I²C-Geräte benötigen passende Spannungspegel und konfliktfreie Adressen |
| UART0 | UART0-TX/RX-Netze an P2 | Mögliche Konflikte mit serieller Protokollausgabe oder anderen UART-Nutzungen; tatsächliche GPIO-Zuordnung prüfen |
| Erweiterung | GPIO45, GPIO46 | Boot-Strapping-Einschränkungen des ESP32-S3; GPIO46 ist zusätzlich eingeschränkt und sollte nicht als uneingeschränkt nutzbarer Ausgang behandelt werden |
| Akku-ADC | GPIO8 | Mit dem Akku-Spannungsteiler verbunden, daher kein unbeschalteter freier Eingang |
| Verstärkerfreigabe | GPIO1 | Intern belegt; aktiv LOW |
| microSD | GPIO5, 4, 6, 7, 2, 3 | Bereits der SD-Schnittstelle zugeordnet |

## 4. Quellenlage und Prüfstatus

| Aussage | Nachweisstatus |
|---|---|
| P2/P3/P4, Pin-Nummern und Netzbezeichnungen | **Originalschaltplan des Herstellers** |
| 5 V an P2; 3,3 V an P3/P4 | **Originalschaltplan des Herstellers** |
| Gemeinsamer I²C-Bus sowie interne Audio- und SD-Verbindungen | **Herstellerschaltplan und Hersteller-Pintabelle** |
| Boot-Einschränkungen von GPIO45/46 | **ESP32-S3-Hardwareeigenschaft**; vor Nutzung Espressif-Dokumentation beachten |
| Tatsächliches Rastermaß, Gegensteckertyp, Pin-1-Ausrichtung und Akku-Polarität am montierten Board | **Am konkreten Gerät noch nicht unabhängig überprüft** |
| Funktion aller externen Steckverbinder am konkreten Gerät | **Noch nicht einzeln getestet**; Display, Touch, Audio und microSD wurden im ESPHome-Projekt separat erfolgreich getestet |

## 5. Quelldokumente und Quellenangaben

1. **LCDWiki-Schaltplan:** `ESP32-S3原理图.pdf` (chinesischer Titel: ESP32-S3-Schaltplan), Blatt 1. Separat bereitgestellter Originalschaltplan. Vor einer Veröffentlichung der PDF selbst die Nutzungsrechte des Herstellers prüfen.
2. **LCDWiki-Pinbelegung:** `Eingefügtes Markdown(3).md` (Übertragung der Hersteller-Pintabelle).
3. **LCDWiki-Produktseite und Dokumentation:** https://www.lcdwiki.com/3.5inch_ESP32-S3_Display
4. **Original-Starterpaket:** `3.5inch_IPS_ESP32-S3_ES3C35P_Quick_Start(1).zip`, mit `3.5inch_ESP32-S3_Quick_Start_Manual.pdf` und weiteren Dateien. Es wird **nicht behauptet**, dass der separat übermittelte Schaltplan in diesem ZIP enthalten ist.
5. **Unabhängige Community-Hardwareanalyse:** https://github.com/jlmeredith/ES3C35P/blob/main/docs/HARDWARE.md – hilfreich zum Abgleich widersprüchlicher Herstellerangaben; kein Ersatz für Tests am konkreten Board.

Diese Dokumentation ist eine eigenständig formulierte Zusammenfassung der genannten Quellen und **keine vom Hersteller zertifizierte Verdrahtungsanleitung**. Vor dem Anschluss externer Hardware die Steckerausrichtung, Spannungen und die jeweilige Platinenrevision überprüfen.
