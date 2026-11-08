# Troubleshooting and lessons learned

1. **Blank LCD after flashing:** disconnect USB/power fully and reconnect; the LCD reset path can survive a software-only reset. Check 40-MHz quad SPI and ST77922 init sequence.
2. **Touch not found:** I²C address is `0x55`, not `0x38`; GPIO38/39, reset GPIO48. A scan at boot may race reset.
3. **Wrong colors:** ESPHome `invert_colors: true`, `color_order: RGB` worked in this driver. Arduino/ESP-IDF settings cannot be copied blindly.
4. **Silent speaker:** GPIO1 amp enable is active LOW; ES8311 is at I²C `0x18`. Check codec and native I²S wiring, not just the WAV file.
5. **Slow WAV playback:** original implementation wrote 48 kHz in WAV but native speaker used an incorrect stream rate. Set `AudioStreamInfo(16, 1, 48000)` in the speaker path; sample width/slot bit width is not the playback speed setting.
6. **No SD mount:** use FAT32 and 4-bit SDMMC pinout; `format_if_mount_failed = false` intentionally protects your data.
7. **`unknown format: 3` in Python `wave`:** WAV type 3 is 32-bit float; use `soundfile` or FFmpeg. The demo writes PCM16 type 1.
8. **`HTTPD_503_SERVICE_UNAVAILABLE` compile error:** ESP-IDF HTTP server does not expose this enum in the tested build. The included header uses `httpd_resp_set_status(req, "503 Service Unavailable")`.
9. **HTTP list/download behavior:** the working implementation opens a known WAV via `fopen`, not directory enumeration. Port 8080 is read-only and has no authentication.
10. **ESPHome build version:** validated logs came from 2026.9.0. This exported genericized configuration was not independently compiled/flashed.

### Useful log checks

```bash
esphome logs es3c35p-hardware-demo.yaml
```

Look for `es8311`, `i2s_audio.microphone`, `i2s_audio.speaker`, `st7123.touchscreen`, `guinea_sd`, `guinea_native`, and `guinea_web`. On a working test unit, ESPHome reported the ES8311 at `0x18`, touch at `0x55`, 8 MB PSRAM and a 320×480 display.
