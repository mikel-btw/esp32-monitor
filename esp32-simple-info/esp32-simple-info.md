# esp32_simple_monitor

Displays the repo link, connected Wi-Fi network name, device IP, and MAC address
on a 0.96" 128x64 IIC OLED display.

## Wiring

| OLED | ESP32-C3 SuperMini |
|------|--------------------|
| VCC  | 3.3V               |
| GND  | GND                |
| SDA  | GPIO20             |
| SCL  | GPIO21             |

No external resistors needed.

## Arduino IDE settings

| Setting          | Value              |
|------------------|--------------------|
| Board            | ESP32C3 Dev Module |
| USB CDC on Boot  | **Enabled**        |
| CPU Frequency    | 160 MHz            |
| Flash Size       | 4MB                |
| Upload Speed     | 921600             |

## Required libraries

Install via **Sketch > Include Library > Manage Libraries**:

| Library              | Author   |
|----------------------|----------|
| Adafruit SSD1306     | Adafruit |
| Adafruit GFX Library | Adafruit |

Built-in (no install needed): `WiFi`, `Wire`

## Configuration

Edit the defines at the top of `esp32_simple_monitor.ino`:

```cpp
#define WIFI_SSID     "your_network"
#define WIFI_PASSWORD "your_password"
```
