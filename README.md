# esp32-mc-monitor

ESP32-C3 SuperMini project that monitors a Minecraft Bedrock server and displays status on a 128x64 OLED.

## Hardware

| Component | Detail |
|-----------|--------|
| Board | ESP32-C3 SuperMini |
| Display | 128x64 OLED SSD1306, I2C |
| SDA | GPIO20 |
| SCL | GPIO21 |
| VCC | 3.3V |
| GND | GND |

## Features

- Connects to Wi-Fi
- Sends a RakNet UnconnectedPing to the Minecraft Bedrock server (UDP port 19132) and measures round-trip latency
- Fetches public IPv4 address from `api.ipify.org`
- Displays everything on the OLED, refreshing every 30 seconds

## OLED layout

```
WiFi: CONNECTED
192.168.x.x
-------------------
MC SERVER
ONLINE          (or OFFLINE)
Ping: 42 ms
-------------------
181.xxx.xxx.xxx
```

## Libraries (install via Arduino IDE Library Manager)

- **Adafruit SSD1306** by Adafruit
- **Adafruit GFX Library** by Adafruit

Built-in (no install needed): `WiFi`, `WiFiUdp`, `WiFiClient`

## Board setup in Arduino IDE

1. File > Preferences > Additional Board Manager URLs:  
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
2. Tools > Board > Boards Manager > search "ESP32" > install **ESP32 by Espressif Systems**
3. Select board: **ESP32C3 Dev Module**
4. Tools > USB CDC on Boot: **Enabled** (required for Serial monitor on ESP32-C3)

## Configuration

Edit the top section of `esp32_mc_monitor.ino`:

```cpp
#define WIFI_SSID        "your_network"
#define WIFI_PASSWORD    "your_password"
```

The Minecraft server host and update interval are also defined there.

## Notes

- Minecraft Bedrock uses RakNet over UDP. A standard ICMP ping to port 19132 will not work. This sketch uses the correct RakNet UnconnectedPing packet.
- If the OLED does not initialize, run an I2C scanner sketch to confirm the address (0x3C is the default for most cheap modules; change `OLED_ADDRESS` to `0x3D` if needed).
- GPIO20 and GPIO21 were chosen because they are general-purpose pins with no conflicts on the SuperMini (no LED, no BOOT button, no JTAG).
