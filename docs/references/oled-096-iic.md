# 0.96" IIC OLED Display (128x64) — Reference

## Overview

Small monochrome OLED display module commonly labeled `0.96" 4P IIC OLED`.  
**IIC and I2C are the same protocol** (Inter-Integrated Circuit) — different name, identical hardware.  
No external resistors needed; pull-up resistors are built into the PCB.

## Key specs

| Property | Value |
|----------|-------|
| Screen size | 0.96 inches diagonal |
| Resolution | 128 x 64 pixels |
| Colors | Monochrome (white or yellow/blue) |
| Controller | SSD1306 (most common) or SH1106 |
| Protocol | I2C / IIC |
| Default I2C address | 0x3C (some modules: 0x3D) |
| Supply voltage | 3.3 V or 5 V (onboard regulator) |
| Pins | VCC, GND, SCL, SDA |
| Pull-up resistors | Built into PCB — no external resistors needed |

## Connector

Module label: `NFP1315-45AY` — this is the FPC/FFC connector part number, not the display controller.

## Pinout

| Pin | Description |
|-----|-------------|
| VCC | Power supply (3.3 V or 5 V) |
| GND | Ground |
| SCL | I2C clock |
| SDA | I2C data |

## Wiring to ESP32-C3 SuperMini

| OLED | ESP32-C3 SuperMini |
|------|--------------------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO20 |
| SCL | GPIO21 |

## I2C address

Most modules ship with address **0x3C**.  
If the display does not initialize, try **0x3D**.  
To confirm, upload an I2C scanner sketch and check the Serial monitor output.

## Arduino libraries

Install via **Sketch > Include Library > Manage Libraries**:

| Library | Author |
|---------|--------|
| Adafruit SSD1306 | Adafruit |
| Adafruit GFX Library | Adafruit |

For SH1106 controllers use **Adafruit SH110X** instead of Adafruit SSD1306.

## Basic usage (Arduino)

```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define I2C_SDA     20
#define I2C_SCL     21
#define OLED_ADDR   0x3C

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Hello");
  display.display();
}
```

## References

- [Adafruit SSD1306 library](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX library](https://github.com/adafruit/Adafruit-GFX-Library)
- [SSD1306 datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
