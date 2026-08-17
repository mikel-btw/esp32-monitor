# ESP32-C3 SuperMini + OLED Projects

A collection of experimental sketches for the **ESP32-C3 SuperMini** paired with a **0.96" 128x64 IIC OLED display**, built with the Arduino IDE. The focus is on learning and experimentation — nothing here is intended for production use.

---

## Hardware

### ESP32-C3 SuperMini

A compact development board based on the ESP32-C3 SoC (RISC-V single-core, 160 MHz). Despite its small size it includes:

- Wi-Fi 802.11 b/g/n (2.4 GHz)
- Bluetooth 5 (LE)
- 400 KB SRAM, 4 MB Flash
- 11 usable GPIO pins
- Hardware I2C, SPI, UART, ADC
- USB-C connector (CDC, used for flashing and Serial monitor)
- Onboard RGB LED (GPIO8)
- BOOT button (GPIO9)
- Operating voltage: 3.3 V logic, powered via USB-C or 5V pin

**Pin conflicts to avoid:**

| GPIO | Conflict |
|------|----------|
| GPIO8 | Onboard RGB LED |
| GPIO9 | BOOT button |
| GPIO4-GPIO7 | JTAG (avoid unless debugging) |
| GPIO11 | Flash (do not use) |
| GPIO12-GPIO17 | Flash (do not use) |

Safe general-purpose pins with no conflicts: **GPIO0, GPIO1, GPIO2, GPIO3, GPIO10, GPIO20, GPIO21**.

---

### 0.96" IIC OLED Display (128x64)

A small monochrome OLED module commonly marked as `0.96" 4P IIC OLED`. IIC and I2C refer to the same protocol (Inter-Integrated Circuit).

| Property | Value |
|----------|-------|
| Resolution | 128 x 64 pixels |
| Controller | SSD1306 (most common) or SH1106 |
| Protocol | I2C / IIC |
| Default I2C address | 0x3C (some modules use 0x3D) |
| Operating voltage | 3.3 V or 5 V (module has onboard regulator) |
| Pins | VCC, GND, SCL, SDA |

**No external resistors needed.** The module has pull-up resistors built into the PCB.

#### Wiring to ESP32-C3 SuperMini

| OLED pin | ESP32-C3 SuperMini |
|----------|--------------------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO20 |
| SCL | GPIO21 |

GPIO20 and GPIO21 are used because they are clean general-purpose pins with no conflicts on this board.

---

## Arduino IDE Setup

### 1. Add ESP32 board support

Go to **File > Preferences > Additional Board Manager URLs** and add:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Then go to **Tools > Board > Boards Manager**, search for `ESP32`, and install **ESP32 by Espressif Systems**.

### 2. Select the correct board and settings

| Setting | Value |
|---------|-------|
| Board | ESP32C3 Dev Module |
| USB CDC on Boot | Enabled |
| CPU Frequency | 160 MHz |
| Flash Size | 4MB |
| Upload Speed | 921600 |

> **USB CDC on Boot must be Enabled** or the Serial monitor will not work on the ESP32-C3.

### 3. Install required libraries

Via **Sketch > Include Library > Manage Libraries**:

| Library | Author |
|---------|--------|
| Adafruit SSD1306 | Adafruit |
| Adafruit GFX Library | Adafruit |

Built-in (no install needed): `WiFi`, `WiFiUdp`, `WiFiClient`, `Wire`

---

## Projects

### `esp32_mc_monitor` — Minecraft Bedrock Server Monitor

Connects to Wi-Fi, checks whether a Minecraft Bedrock server is reachable, measures its response time, fetches the device's public IP address, and displays everything on the OLED. Refreshes every 30 seconds without blocking.

**How the Minecraft check works:**
Minecraft Bedrock uses the RakNet protocol over UDP on port 19132. A standard ICMP ping does not work. This sketch sends a RakNet `UnconnectedPing` packet and waits for an `UnconnectedPong` reply, measuring the round-trip time.

**OLED layout:**
```
WiFi: CONNECTED
192.168.x.x
-------------------
MC SERVER
ONLINE
Ping: 42 ms
-------------------
181.xxx.xxx.xxx
```

**Configuration** — edit the top of the `.ino` file:
```cpp
#define WIFI_SSID     "your_network"
#define WIFI_PASSWORD "your_password"
#define MC_HOST       "your.server.hostname"
```

---

## Notes

- If the OLED does not initialize, the I2C address may be `0x3D` instead of `0x3C`. Change `OLED_ADDRESS` in the sketch, or run an I2C scanner sketch first to confirm.
- If you see garbled output on Serial monitor, make sure **USB CDC on Boot** is set to **Enabled** in the board settings.
- All sketches in this repo are written for experimentation. Wi-Fi credentials are hardcoded — do not share your `.ino` files publicly with real credentials in them.
