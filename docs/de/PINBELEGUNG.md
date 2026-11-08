# Pinbelegung

| Peripheral | Signal | GPIO/address | Status/Nachweis |
|---|---|---|---|
| ST77922 LCD | CS / CLK | 10 / 12 | tested |
| ST77922 LCD | D0 / D1 / D2 / D3 | 11 / 13 / 14 / 9 | tested |
| Backlight | PWM | 41 | tested |
| Touch | I²C SDA / SCL | 38 / 39 | tested |
| Touch | I²C address | **0x55** | tested; some vendor PDFs erroneously show `0x38` |
| Touch | reset / interrupt | 48 / 47 | reset tested; IRQ not used in demo |
| ES8311 codec | I²C address | **0x18** | tested |
| I²S | MCLK / BCLK / LRCLK | 17 / 18 / 21 | tested |
| I²S | codec -> MCU (DIN) | 16 | tested recording |
| I²S | MCU -> codec (DOUT) | 15 | tested playback |
| SC8002B amp | enable, active LOW | 1 | tested |
| SDMMC | CLK / CMD | 5 / 4 | tested |
| SDMMC | D0 / D1 / D2 / D3 | 6 / 7 / 2 / 3 | tested |
| RGB LED | data | 40 | vendor/source only, not tested |
| Boot | button | 0 | vendor/source |
| Battery | voltage ADC | 8 | vendor/source only |
| UART0 | TX / RX | 43 / 44 | source reference; vendor tables may swap |
| Expansion | pins | 45 / 46 | vendor/source only; check strapping constraints |

The `i2c.scan` may show `0x18`, `0x55`, and sometimes `0x28`; the latter was observed in a log but **has not been identified** here. Do not infer a component from a single scan.

Display uses `model: CUSTOM`, `bus_mode: quad`, 40 MHz, `invert_colors: true`, `color_order: RGB`, and a vendor-derived init sequence. Native portrait 320×480; LVGL rotation 0. For landscape, follow the tested external reference; don't simply change the display rotation while LVGL is active.

### Source and measurement distinctions

- **Tested on our unit:** display/touch, SD mount, audio record/playback, 48 kHz, web download.
- **Other developer:** `jlmeredith/ES3C35P` confirms display/touch and reset caveats.
- **Vendor/source only:** untested battery, RGB LED, expansion and IRQ operation.

**Hinweis:** „tested“ bedeutet am eigenen Gerät praktisch geprüft; „vendor/source only“ bedeutet nur aus Unterlagen/Quellcode abgeleitet.
