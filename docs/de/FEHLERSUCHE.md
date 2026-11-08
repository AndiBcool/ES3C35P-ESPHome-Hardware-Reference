# Fehlersuche / Erkenntnisse aus unseren Tests

1. **Schwarzes Display:** Nach Firmwarewechsel USB/Strom vollständig trennen und neu anschließen. QSPI-Pins, 40 MHz und ST77922-Initialisierung prüfen.
2. **Touch fehlt:** Adresse `0x55` (nicht `0x38` aus einer fehlerhaften Hersteller-PDF), GPIO38/39 und Reset GPIO48.
3. **Falsche Farben:** Bei ESPHome funktionierten `invert_colors: true` und `color_order: RGB`. Werte aus anderen Treibern nicht blind übertragen.
4. **Lautsprecher stumm:** Verstärker GPIO1 **LOW aktiv**, Codec-Adresse `0x18`, I²S prüfen.
5. **Wiedergabe verlangsamt:** 48-kHz-WAV-Header allein reicht nicht. `AudioStreamInfo(16, 1, 48000)` im nativen Lautsprecherpfad setzen. `Slot bit width: 16` ist keine Abtastrate.
6. **microSD nicht gemountet:** FAT32, 4-Bit-SDMMC-Pins prüfen. `format_if_mount_failed = false` verhindert Datenverlust.
7. **Python `wave` meldet `unknown format: 3`:** Float-WAV; `soundfile` oder FFmpeg nutzen. Unser Beispiel schreibt PCM16.
8. **HTTP-503-Kompilierfehler:** `HTTPD_503_SERVICE_UNAVAILABLE` existierte im verwendeten ESP-IDF-Header nicht. Die mitgelieferte Datei verwendet `httpd_resp_set_status(req, "503 Service Unavailable")`.
9. **Webdownload:** Die erprobte Variante öffnet `aufnahme.wav` direkt per `fopen`, keine Ordnerauflistung. Port 8080 ist nicht authentifiziert.
10. **Versionen:** ESPHome 2026.9.0 auf dem Testgerät; das neutralisierte Demo wurde noch nicht unabhängig kompiliert.

```bash
esphome logs es3c35p-hardware-demo.yaml
```

Relevante Lognamen: `es8311`, `i2s_audio.microphone`, `i2s_audio.speaker`, `st7123.touchscreen`, `guinea_sd`, `guinea_native`, `guinea_web`.
