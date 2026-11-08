# Herkunft und Prüfumfang

Die Hardwarebelegung wurde aus LCDWiki-Dokumentation, Xiaozhi-Boardcode und `jlmeredith/ES3C35P` zusammengeführt und an einem eigenen Gerät teilweise praktisch bestätigt. Die YAML basiert auf dem erprobten 48-kHz-Prototyp einschließlich des korrigierten Lautsprecher-Streams. **Die neutralisierte Fassung wurde noch nicht eigenständig kompiliert oder geflasht.**

## ST77922-Initialisierung: Quellen und Einordnung

Die verwendete ST77922-Registerfolge ist in den **LCDWiki-ES3C35P-Beispielen** dokumentiert. Das Projekt **[jlmeredith/ES3C35P](https://github.com/jlmeredith/ES3C35P)** beschreibt dieselbe Tabelle außerdem in der ausgelieferten Gerätefirmware und stellt eine ESPHome-kompatible Darstellung bereit. Unsere YAML folgt dieser dokumentierten Registerfolge (einschließlich der charakteristischen Befehle `0x80`, `0xB9`, `0x2A`, `0x2B` und `0x11`). Die konkrete historische Übernahmekette unserer früheren Arbeitsversion lässt sich nicht zweifelsfrei rekonstruieren.

Quellen:
- [LCDWiki: ES3C35P-Produktseite und Startermaterial](https://www.lcdwiki.com/3.5inch_ESP32-S3_Display)
- [LCDWiki: ES3C35P-Dateiverzeichnis](https://www.lcdwiki.com/res/ES3C35P/)
- [jlmeredith/ES3C35P: Hardwareanalyse, Sequenz und Lizenzhinweise](https://github.com/jlmeredith/ES3C35P)
- [xiaozhi-esp32: ES3C35P-Boardimplementierung](https://github.com/78/xiaozhi-esp32/tree/main/main/boards/lcdwiki-es3c35p)

Die Registertabelle wird als **technische Information zur Hardware-Kompatibilität** bereitgestellt. jlmeredith veröffentlicht den übrigen eigenen Projektcode unter MIT und beansprucht nach eigener Dokumentation keine eigenen Rechte an der aus Herstellerunterlagen stammenden Tabelle. Daraus folgt **keine automatische MIT-Lizenz für LCDWiki-Materialien**. Etwaige Lizenztexte und Hinweise fremder Codebestandteile sind bei deren Übernahme gesondert zu beachten.

**Nicht enthalten:** originale Hersteller-Firmware, vollständige Flashabbilder und das LCDWiki-Starterpaket. Eine rechtlich verbindliche Freigabe sämtlicher Herstellerinhalte wird nicht behauptet.
