# ES3C35P / 3.5-inch ESP32-S3 — External Connectors and Pin Reference

This document summarizes the board's external connections using the **manufacturer's electrical schematic** (`ESP32-S3原理图.pdf`, sheet 1) and the **manufacturer's ESP32-S3 pin allocation table** (`Eingefügtes Markdown(3).md`, copied from LCDWiki). It describes **electrical net labels and schematic pin numbers**, not a verified physical viewing orientation or connector pitch/type.

## Connector Locations – PCB Rear View

The following photo shows the rear side of the ES3C35P board,
including the printed connector labels.

![ES3C35P PCB rear view](../images/ES3C35P_Connectors.jpg)

> **Safety:** Disconnect power before wiring. **P2 pin 1 is +5 V, whereas P3 and P4 pin 1 are 3.3 V.** Do not assume that the numbering is left-to-right when looking at the back of the assembled board; confirm the physical pin-1 marking and orientation. Do not connect 5 V directly to ESP32-S3 GPIOs.

## 1. Four-pin external connectors

| Connector | Schematic label | Pin | Net / board label | ESP32-S3 signal | Description | Supply / logic |
|---|---|---:|---|---|---|---|
| UART | **P2** | 1 | `+5` | — | 5 V power rail | **5 V** |
| UART | **P2** | 2 | `GND` | — | Ground | 0 V |
| UART | **P2** | 3 | `TXD0` | UART0 TX | Serial transmit net (via 100 Ω series resistor) | 3.3 V logic |
| UART | **P2** | 4 | `RXD0` | UART0 RX | Serial receive net (via 100 Ω series resistor) | 3.3 V logic |
| Expansion GPIO | **P3** | 1 | `VCC3V3` | — | 3.3 V power rail | **3.3 V** |
| Expansion GPIO | **P3** | 2 | `GND` | — | Ground | 0 V |
| Expansion GPIO | **P3** | 3 | `IO46` | GPIO46 | Expansion signal; boot-strapping restrictions | 3.3 V logic |
| Expansion GPIO | **P3** | 4 | `IO45` | GPIO45 | Expansion signal; boot-strapping restrictions | 3.3 V logic |
| I²C | **P4** | 1 | `VCC3V3` | — | 3.3 V power rail | **3.3 V** |
| I²C | **P4** | 2 | `GND` | — | Ground | 0 V |
| I²C | **P4** | 3 | `IIC_SCL` | GPIO39 | Shared I²C clock | 3.3 V logic |
| I²C | **P4** | 4 | `IIC_SDA` | GPIO38 | Shared I²C data | 3.3 V logic |

**Source:** `ESP32-S3原理图.pdf`, sheet 1, lower-left section labelled `外部接口` (external interfaces), with P2 (`串口`, serial), P3 (`扩展IO`, expansion I/O), and P4 (`I2C`). The LCDWiki pin-allocation table independently describes GPIO38/39 and GPIO45/46.

### UART GPIO-number caution

The manufacturer pin-allocation text labels `RXD0(IO43)` and `TXD0(IO44)`, while another ES3C35P hardware reference reports the UART directions differently. **The schematic unambiguously identifies the connector nets `TXD0` and `RXD0`, but the UART-to-GPIO-number mapping should be verified against the specific board revision before documenting it as tested.** Use the **net labels** above for connector wiring; connect an external device's RX to `TXD0` and its TX to `RXD0`, with a common ground and 3.3 V logic.

## 2. Other connections and circuitry

| Schematic designator | Net(s) / component | Function | Notes |
|---|---|---|---|
| **JP1** | `BAT+`, `GND` | External battery connection | Battery charging/discharging section; confirm polarity at the physical connector before use |
| **U2** | `TP4054_C668215` | Single-cell lithium battery charge-control circuit | The presence of a charger does **not** establish a suitable cell, connector polarity, protection circuit, or safe charge current for a particular battery |
| **GPIO8** | `BAT_ADC` | Battery-voltage measurement | Schematic shows R14/R15, both 100 kΩ, as a voltage divider (nominal ADC input approximately half the battery voltage) |
| **JP3** | `SP+`, `SP-` | External speaker connection | Differential output of the SC8002B amplifier; **neither speaker terminal should be assumed to be GND** |
| **U5** | `ES8311` | Audio codec | I²S audio interface and shared I²C control |
| **U6** | `SC8002B` | Speaker power amplifier | `AUDIO_EN` driven by GPIO1, active LOW per pin-allocation table |
| **SD_CARD1** | `SD_CLK`, `SD_CMD`, `SD_D0`–`SD_D3` | microSD slot | GPIO5, GPIO4, GPIO6/7/2/3 respectively |
| **USB2** | USB-C, `VBUS`, `USB+`, `USB-` | USB power and data | USB power is not the same as the 3.3 V GPIO supply |

**Source:** `ESP32-S3原理图.pdf`, sheet 1, sections `电池充放电电路` (battery charging/discharging), `音频` (audio), `TF卡` (microSD), and `USB接口` (USB).

## 3. Shared bus and GPIO reservations

| Function | GPIOs | Consequence for external expansion |
|---|---|---|
| Shared I²C | GPIO38 SDA, GPIO39 SCL | Touch controller and ES8311 codec already share P4; additional I²C devices need compatible voltage and non-conflicting addresses |
| UART0 | UART0 TX/RX nets on P2 | May conflict with UART0 logging or other serial uses; check the actual GPIO mapping |
| Expansion | GPIO45, GPIO46 | ESP32-S3 strapping-pin restrictions; GPIO46 has additional limitations, so avoid treating these as unrestricted general-purpose outputs |
| Battery ADC | GPIO8 | Connected to battery-voltage divider, not a free general-purpose input |
| Audio amplifier enable | GPIO1 | Active LOW; used internally |
| microSD | GPIO5, 4, 6, 7, 2, 3 | Already assigned to the SD interface |

## 4. Evidence and validation status

| Item | Evidence level |
|---|---|
| P2/P3/P4 connector designators, schematic pin numbers, net names | **Manufacturer schematic** |
| 5 V on P2; 3.3 V on P3/P4 | **Manufacturer schematic** |
| Shared I²C and internal audio/SD connections | **Manufacturer schematic + manufacturer pin-allocation table** |
| GPIO45/46 boot restrictions | **ESP32-S3 hardware constraint**; consult Espressif documentation before use |
| Actual header pitch, mating connector type, physical pin-1 orientation, and battery polarity on the assembled unit | **Not independently verified on the user's board** |
| Full external connector functionality on the user's particular board | **Not tested**; display, touch, audio, and microSD were tested separately in the ESPHome project |

## 5. Source documents and attribution

1. **LCDWiki board schematic:** `ESP32-S3原理图.pdf` (Chinese title: ESP32-S3 schematic), sheet 1. Manufacturer schematic supplied separately; if redistributing it, check the manufacturer's reuse terms.
2. **LCDWiki ESP32-S3 pin allocation:** `Eingefügtes Markdown(3).md` (transcription of the manufacturer's pin allocation table).
3. **LCDWiki 3.5-inch ESP32-S3 product and documentation page:** https://www.lcdwiki.com/3.5inch_ESP32-S3_Display
4. **Manufacturer quick-start archive:** `3.5inch_IPS_ESP32-S3_ES3C35P_Quick_Start(1).zip`; includes `3.5inch_ESP32-S3_Quick_Start_Manual.pdf` and related starter material. The separately supplied schematic is **not claimed** to be included in this archive.
5. **Independent community hardware notes:** https://github.com/jlmeredith/ES3C35P/blob/main/docs/HARDWARE.md — useful for cross-checking manufacturer discrepancies; not a substitute for board-level validation.

This is an independently written reference derived from the cited documents, not a manufacturer-certified wiring guide. Before connecting external hardware, verify connector orientation, voltage, and the specific board revision.
