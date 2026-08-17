# ESP32-C3 SuperMini — Reference

## Overview

Compact development board based on the **ESP32-C3** SoC by Espressif.  
Single-core RISC-V at 160 MHz. Built-in Wi-Fi and Bluetooth LE.  
Powered via USB-C. 3.3 V logic.

## Key specs

| Property | Value |
|----------|-------|
| CPU | RISC-V single-core, 160 MHz |
| SRAM | 400 KB |
| Flash | 4 MB |
| Wi-Fi | 802.11 b/g/n, 2.4 GHz |
| Bluetooth | BT 5, BLE |
| GPIO | 11 usable |
| Logic voltage | 3.3 V |
| Power input | USB-C or 5V pin |
| Onboard LED | RGB on GPIO8 |
| BOOT button | GPIO9 |

## Pinout
            USB-C
        ┌─────┴─────┐
5V     ─┤1        16├─ GND
GND    ─┤2        15├─ 3.3V
3.3V   ─┤3        14├─ GPIO4  (JTAG: TMS)
GPIO2  ─┤4        13├─ GPIO5 (JTAG: TDI)
GPIO3  ─┤5        12├─ GPIO6 (JTAG: TCK)
GPIO10 ─┤6        11├─ GPIO7 (JTAG: TDO)
GPIO20 ─┤7        10├─ GPIO8 (RGB LED)
GPIO21 ─┤8         9├─ GPIO9 (BOOT button)
        └───────────┘


## Pin conflicts

| GPIO | Conflict | Usable? |
|------|----------|---------|
| GPIO4 | JTAG TMS | Avoid unless debugging |
| GPIO5 | JTAG TDI | Avoid unless debugging |
| GPIO6 | JTAG TCK | Avoid unless debugging |
| GPIO7 | JTAG TDO | Avoid unless debugging |
| GPIO8 | Onboard RGB LED | With care |
| GPIO9 | BOOT button | With care |
| GPIO11 | Flash | Do not use |
| GPIO12-17 | Flash | Do not use |

**Clean pins with no conflicts: GPIO0, GPIO1, GPIO2, GPIO3, GPIO10, GPIO20, GPIO21**

## Arduino IDE settings

| Setting | Value |
|---------|-------|
| Board | ESP32C3 Dev Module |
| USB CDC on Boot | **Enabled** (required for Serial monitor) |
| CPU Frequency | 160 MHz |
| Flash Size | 4MB |
| Upload Speed | 921600 |

## Board manager URL

https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json


## References

- [Tutorial-ESP32-C3-Super-Mini by sidharthmohannair](https://github.com/sidharthmohannair/Tutorial-ESP32-C3-Super-Mini)
- [Espressif ESP32-C3 datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
- [arduino-esp32 by Espressif](https://github.com/espressif/arduino-esp32)
