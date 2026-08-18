/*
 * esp32_simple_monitor.ino
 *
 * ESP32-C3 SuperMini - Simple Network Info Monitor
 *
 * Displays on OLED:
 *   - Project repo link
 *   - Connected Wi-Fi network name (SSID)
 *   - Device IP address
 *   - Device MAC address
 *
 * Hardware:
 *   Board  : ESP32-C3 SuperMini
 *   Display: 128x64 OLED SSD1306 via I2C
 *   SDA    : GPIO20
 *   SCL    : GPIO21
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------
#define WIFI_SSID      "FLIA OSORIO ORDUZ"
#define WIFI_PASSWORD  "E63503269"

// ---------------------------------------------------------------------------
// I2C pins
// ---------------------------------------------------------------------------
#define I2C_SDA  20
#define I2C_SCL  21

// ---------------------------------------------------------------------------
// OLED
// ---------------------------------------------------------------------------
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("SSD1306 not found. Check wiring and I2C address.");
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected.");
  } else {
    Serial.println("\nFailed to connect.");
  }

  drawDisplay();
}

// ---------------------------------------------------------------------------
// Loop - nothing to do, display is static
// ---------------------------------------------------------------------------
void loop() {
  // Reconnect if dropped and redraw
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
      delay(500);
    }
    drawDisplay();
  }

  delay(5000);
}

// ---------------------------------------------------------------------------
// Draw display
//
//  y=0  : github.com/mikel-btw/
//  y=10 : esp32-monitor
//  y=22 : divider
//  y=25 : SSID
//  y=37 : IP address
//  y=49 : MAC address
// ---------------------------------------------------------------------------
void drawDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Repo link
  display.setCursor(0, 0);
  display.println("github.com/mikel-btw/");
  display.setCursor(0, 10);
  display.println("esp32-monitor");

  // Divider
  display.drawFastHLine(0, 21, SCREEN_WIDTH, SSD1306_WHITE);

  if (WiFi.status() == WL_CONNECTED) {
    // SSID
    display.setCursor(0, 25);
    display.println(WiFi.SSID());

    // IP
    display.setCursor(0, 37);
    display.println(WiFi.localIP().toString());

    // MAC
    display.setCursor(0, 49);
    display.println(WiFi.macAddress());
  } else {
    display.setCursor(0, 30);
    display.println("WiFi not connected");
  }

  display.display();
}
