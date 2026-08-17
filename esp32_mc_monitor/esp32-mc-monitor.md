# esp32_mc_monitor

Sketch for the ESP32-C3 SuperMini that monitors a Minecraft Bedrock server
and displays live status on a 0.96" 128x64 IIC OLED display.

## What it does

- Connects to Wi-Fi
- Sends a RakNet UnconnectedPing to the Minecraft Bedrock server (UDP port 19132)
- Measures round-trip response time in milliseconds
- Fetches the device's public IPv4 address from `api.ipify.org`
- Displays everything on the OLED, refreshing every 30 seconds

## Why UDP and not ICMP ping

Minecraft Bedrock runs on the RakNet protocol over UDP port 19132.
A standard ICMP ping tells you nothing about whether the Minecraft server
is actually running. This sketch sends the correct RakNet `UnconnectedPing`
packet and waits for an `UnconnectedPong` reply — the same handshake a
Bedrock client uses to check if a server is alive.

## OLED layout

WiFi: CONNECTED
192.168.x.x
───────────────────
MC SERVER
ONLINE
Ping: 42 ms
───────────────────
181.xxx.xxx.xxx


## Configuration

Edit the defines at the top of `esp32_mc_monitor.ino`:

```cpp
#define WIFI_SSID        "your_network"
#define WIFI_PASSWORD    "your_password"
#define MC_HOST          "your.server.hostname"
#define UPDATE_INTERVAL_MS  30000   // refresh interval in milliseconds
```

## Hardware

| Component | Detail |
|-----------|--------|
| Board | ESP32-C3 SuperMini |
| Display | 0.96" 128x64 IIC OLED (SSD1306) |
| SDA | GPIO20 |
| SCL | GPIO21 |

Full wiring and hardware details in [`docs/references/`](../docs/references/).

## Required libraries

Install via Arduino IDE Library Manager:

| Library | Author |
|---------|--------|
| Adafruit SSD1306 | Adafruit |
| Adafruit GFX Library | Adafruit |

Built-in (no install needed): `WiFi`, `WiFiUdp`, `WiFiClient`, `Wire`
