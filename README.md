# LCDWiki ES3C35P / WS-32871 — ESPHome hardware reference

**English documentation** · [Deutsch](docs/de/README.md) · [Pinout](docs/en/PINOUT.md) · [Troubleshooting](docs/en/TROUBLESHOOTING.md)

A practical ESPHome hardware bring-up and audio/SD reference for the **3.5-inch ESP32-S3 display board** sold as LCDWiki **ES3C35P** (and some WS-32871/rebranded units). This is a **hardware reference with the Robby mascot demo UI**: a full-screen illustration, LVGL touch buttons, ES8311 audio, 4-bit SDMMC WAV recording, playback, and a small read-only WAV download page. The mascot is a demonstration asset, not a required hardware component.

**Status (2026-11-08):** The underlying board features were exercised on one physical ESP32-S3 using **ESPHome 2026.9.0** (ESP-IDF). Display, touch, microSD, native I²S recording, speaker playback and HTTP WAV download were observed working. The **demo YAML preserves the tested 48-kHz hardware settings, but this renamed/repackaged repository version has not itself been compiled/flashed on a board**. In particular, treat this as a reproducible reference, not a pre-certified firmware release. Other board revisions may differ.

## Features / evidence

| Feature | Configuration | Evidence |
| --- | --- | --- |
| ESP32-S3 | 16 MB flash, 8 MB octal PSRAM | Device probe and ESPHome log |
| LCD | ST77922, 320×480, QSPI @ 40 MHz | Working on device |
| Touch | `st7123`, I²C `0x55`, GPIO48 reset | Working on device |
| Backlight | GPIO41 LEDC PWM | Working on device |
| ES8311 codec | I²C `0x18`, native ESPHome I²S | Audio capture/playback observed |
| Microphone | 48 kHz, mono, 16-bit PCM | Recorded WAV inspected/listened to |
| Speaker | 48 kHz mono, 16-bit PCM | Playback fixed and reported correct |
| Amplifier | GPIO1 **active LOW** | Playback tested |
| microSD | 4-bit SDMMC, FAT32, `/sdcard` | 32 GB card mounted and WAV saved |
| WAV HTTP | Port 8080, one known `aufnahme.wav` | Download tested |
| WS2812 LED, battery, expansion | See pinout | Not tested in this reference |

## Repository layout

```text
README.md                              English landing page
esphome/es3c35p-hardware-demo.yaml    Hardware demo with Robby mascot and touch controls
esphome/robby_display.png             Robby mascot screen image
esphome/includes/guinea_sd_test.h     SDMMC/FAT mount helper (legacy namespace)
esphome/includes/guinea_native_wav.h  48-kHz native I²S WAV recorder/player
esphome/includes/guinea_wav_web.h     HTTP download helper, port 8080
docs/en/                             English detailed notes
docs/de/                             Full German documentation
```

The C++ helper filenames and namespaces retain their original `guinea_*` identifiers for compatibility with the tested code. These are internal identifiers; the example is a standalone hardware demo.

## Getting started (ESPHome)

1. Back up any factory firmware **before flashing**. Do **not** publish raw flash dumps: they may contain Wi-Fi credentials and other secrets.
2. Use ESPHome **2026.9.0** or a compatible release with `mipi_spi`, `st7123`, `es8311`, native `microphone`/`speaker` and LVGL. ESP-IDF framework required by this example.
3. Copy `esphome/es3c35p-hardware-demo.yaml` into your ESPHome configuration directory. Keep the `includes/` directory beside it. Provide `wifi_ssid` and `wifi_password` in your local ESPHome `secrets.yaml` (not in Git).
4. Insert a FAT32 microSD card. The helper will **not format** a card if mounting fails.
5. Validate, compile and flash, using the real config path and device serial port:

```bash
esphome config es3c35p-hardware-demo.yaml
esphome compile es3c35p-hardware-demo.yaml
esphome run es3c35p-hardware-demo.yaml
esphome logs es3c35p-hardware-demo.yaml
```

The ESPHome Dashboard's **Install** and **Logs** buttons provide equivalent workflows. The initial flash may require a USB cable and bootloader mode; subsequent updates can use OTA once configured.

### Using the demo

The supplied screen image is `esphome/robby_display.png` (included in this repository). It is displayed behind the record/playback touch controls. This image is part of the sample interface, not a hardware requirement.


- Tap **RECORD** to start recording; tap again to stop. Saves `/sdcard/aufnahme.wav` (overwrites the previous recording).
- Tap **PLAY** to replay through the speaker. The amp enable line is inverted/active LOW.
- Set speaker volume and mic ADC gain from ESPHome/Home Assistant entities. Mic gain uses ES8311 register `0x17` (0–32 dB control as used in the tested prototype).
- Open `http://<device-ip>:8080/` to download `aufnahme.wav` on your local network. **No authentication/TLS**: do not expose port 8080 to the Internet.

**48 kHz playback gotcha:** A 48-kHz WAV header by itself is insufficient. The native speaker stream must also receive `AudioStreamInfo(16, 1, 48000)`; otherwise the audio may play too slowly. See `guinea_native_wav.h`.

**Important:** 48 kHz sampling permits representation of frequencies up to 24 kHz *in principle*. The analog microphone/codec frequency response was **not calibrated**. An acoustic comparison found recognizable high-frequency patterns, not a certified flat response.

## Compared with prior work

Thanks to **[jlmeredith/ES3C35P](https://github.com/jlmeredith/ES3C35P)** for carefully sourced board research and a working **display + touch** ESPHome bring-up. That project documents vendor inconsistencies, the ST77922 init sequence, LCD reset/power-cycle caveat, and a diagnostic/LVGL example. This repository adds the **locally tested ES8311 mic/speaker path, native I²S 48-kHz WAV capture/playback, 4-bit microSD mounting, ADC gain controls and simple HTTP download**. The upstream example is more focused on hardware provenance and display/touch diagnostics; ours emphasizes integrated peripherals and end-to-end audio. We do not claim these features are unsupported upstream or that our generic demo was independently regression-tested.

## Vendor and upstream references

- [LCDWiki official board page / quick-start / schematic / starter package](https://www.lcdwiki.com/3.5inch_ESP32-S3_Display)
- [LCDWiki downloadable files directory](https://www.lcdwiki.com/res/ES3C35P/)
- [jlmeredith/ES3C35P](https://github.com/jlmeredith/ES3C35P) — detailed display/touch investigation
- [Xiaozhi ESP32 LCDWiki board implementation](https://github.com/78/xiaozhi-esp32/tree/main/main/boards/lcdwiki-es3c35p)
- [ESPHome `mipi_spi`](https://esphome.io/components/display/mipi_spi/), [`st7123`](https://esphome.io/components/touchscreen/st7123/)

## License and Robby image

Original project code and documentation are provided under the [MIT License](LICENSE), **to the extent that the repository maintainer holds the necessary rights**. Third-party components retain their respective notices and licenses; see [provenance](docs/en/PROVENANCE.md).

**Exception — Robby illustration:** `esphome/robby_display.png` is **not** covered by the MIT License. **Copyright © 2026 AndiBcool. All rights reserved.** Permission is granted to download and redistribute the unmodified image **only as part of this unmodified ES3C35P demo repository**, and to use it for running, testing, and demonstrating the included ES3C35P hardware demo. Use in other projects, standalone redistribution, modifications, or commercial exploitation of the image require prior written permission. This exception does not restrict the MIT-licensed code. See [Robby image license](esphome/ROBBY-LICENSE.md).

## Publication and safety

Only source/configuration is included: **no vendor firmware, original flash dump, copyrighted starter ZIP, private Wi-Fi credentials or animal recordings**. The ST77922 initialization register table is documented in LCDWiki ES3C35P examples and is also reported in the factory firmware by [jlmeredith/ES3C35P](https://github.com/jlmeredith/ES3C35P). Our ESPHome-format sequence follows the register sequence documented there; see [provenance notes](docs/en/PROVENANCE.md). The table is hardware interoperability data, not a redistribution of manufacturer firmware. This provenance does not itself establish a blanket license for all vendor material. The Robby image is separately licensed as described above. The repository does not claim a license for third-party vendor materials. Contributions welcome after reproducible testing.
