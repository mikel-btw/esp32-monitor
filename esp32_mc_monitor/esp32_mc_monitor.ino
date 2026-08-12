/*
 * esp32_mc_monitor.ino
 *
 * ESP32-C3 SuperMini - Minecraft Bedrock Server Monitor
 *
 * Hardware:
 *   Board  : ESP32-C3 SuperMini
 *   Display: 128x64 OLED SSD1306 via I2C
 *   SDA    : GPIO20
 *   SCL    : GPIO21
 *
 * Libraries required (install via Arduino IDE Library Manager):
 *   - Adafruit SSD1306
 *   - Adafruit GFX Library
 *
 * Built-in (no install needed):
 *   - WiFi.h
 *   - WiFiUdp.h
 *   - WiFiClient.h
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>

// ---------------------------------------------------------------------------
// Configuration - edit these values
// ---------------------------------------------------------------------------
#define WIFI_SSID        "FLIA OSORIO ORDUZ"
#define WIFI_PASSWORD    "E63503269"

#define MC_HOST          "bedrock.zonneczun.dpdns.org"
#define MC_PORT          19132

#define PUBLIC_IP_HOST   "api.ipify.org"
#define PUBLIC_IP_PORT   80

#define UPDATE_INTERVAL_MS  30000   // refresh every 30 seconds

// ---------------------------------------------------------------------------
// I2C pins (GPIO20=SDA, GPIO21=SCL are the clean general-purpose pins)
// ---------------------------------------------------------------------------
#define I2C_SDA  20
#define I2C_SCL  21

// ---------------------------------------------------------------------------
// OLED
// ---------------------------------------------------------------------------
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1   // no reset pin
#define OLED_ADDRESS  0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------------------------------------------------------------------
// RakNet UnconnectedPing packet (Bedrock server status check)
// Spec: https://wiki.bedrock.dev/servers/raknet
//   Byte 0   : 0x01 (ID_UNCONNECTED_PING)
//   Bytes 1-8: client time (ms, little-endian unsigned long long)
//   Bytes 9-24: Magic (fixed: 00ffff00fefefefefdfdfdfd12345678)
//   Bytes 25-32: client GUID (any 8 bytes)
// ---------------------------------------------------------------------------
static const uint8_t RAKNET_MAGIC[16] = {
  0x00, 0xFF, 0xFF, 0x00,
  0xFE, 0xFE, 0xFE, 0xFE,
  0xFD, 0xFD, 0xFD, 0xFD,
  0x12, 0x34, 0x56, 0x78
};

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
String  publicIP       = "...";
bool    mcOnline       = false;
int32_t mcPingMs       = -1;
bool    wifiConnected  = false;

unsigned long lastUpdateMs = 0;

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
void connectWiFi();
void runUpdate();
bool pingBedrockServer(const char* host, uint16_t port, int32_t& pingMs);
bool fetchPublicIP(String& ip);
void drawDisplay();
void drawCentered(const char* text, int16_t y, uint8_t size = 1);

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
  drawCentered("MC Monitor", 20, 1);
  drawCentered("Connecting WiFi...", 40, 1);
  display.display();

  connectWiFi();

  // Force first update immediately
  lastUpdateMs = millis() - UPDATE_INTERVAL_MS;
}

// ---------------------------------------------------------------------------
// Loop
// ---------------------------------------------------------------------------
void loop() {
  // Reconnect if dropped
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    connectWiFi();
  }

  unsigned long now = millis();
  if (now - lastUpdateMs >= UPDATE_INTERVAL_MS) {
    lastUpdateMs = now;
    runUpdate();
  }
}

// ---------------------------------------------------------------------------
// Connect to WiFi (blocking with timeout)
// ---------------------------------------------------------------------------
void connectWiFi() {
  Serial.printf("Connecting to %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.printf("\nWiFi connected. Local IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    wifiConnected = false;
    Serial.println("\nWiFi connection failed.");
  }

  drawDisplay();
}

// ---------------------------------------------------------------------------
// Run a full data refresh cycle
// ---------------------------------------------------------------------------
void runUpdate() {
  Serial.println("Updating...");

  if (!wifiConnected) {
    drawDisplay();
    return;
  }

  // Show "updating" state while fetching
  display.clearDisplay();
  display.setTextSize(1);
  drawCentered("Updating...", 28);
  display.display();

  mcOnline = pingBedrockServer(MC_HOST, MC_PORT, mcPingMs);
  fetchPublicIP(publicIP);

  Serial.printf("MC online: %s  ping: %d ms\n", mcOnline ? "YES" : "NO", mcPingMs);
  Serial.printf("Public IP: %s\n", publicIP.c_str());

  drawDisplay();
}

// ---------------------------------------------------------------------------
// Send RakNet UnconnectedPing, wait for UnconnectedPong (0x1C), measure RTT
// Returns true if server replied within timeout
// ---------------------------------------------------------------------------
bool pingBedrockServer(const char* host, uint16_t port, int32_t& pingMs) {
  pingMs = -1;

  // Resolve hostname
  IPAddress serverIP;
  if (!WiFi.hostByName(host, serverIP)) {
    Serial.printf("DNS failed for %s\n", host);
    return false;
  }
  Serial.printf("Resolved %s -> %s\n", host, serverIP.toString().c_str());

  WiFiUDP udp;
  if (!udp.begin(0)) {   // 0 = OS picks a local port
    Serial.println("UDP begin failed");
    return false;
  }

  // Build UnconnectedPing (33 bytes total)
  uint8_t packet[33];
  packet[0] = 0x01;  // ID_UNCONNECTED_PING

  uint64_t timeMs = (uint64_t)millis();
  memcpy(&packet[1], &timeMs, 8);         // client time, little-endian
  memcpy(&packet[9], RAKNET_MAGIC, 16);   // magic

  uint64_t guid = 0xDEADBEEFCAFEBABEULL;
  memcpy(&packet[25], &guid, 8);          // client GUID

  unsigned long sendTime = micros();
  udp.beginPacket(serverIP, port);
  udp.write(packet, sizeof(packet));
  if (!udp.endPacket()) {
    Serial.println("UDP send failed");
    udp.stop();
    return false;
  }

  // Wait up to 3 seconds for a reply
  bool received = false;
  uint32_t deadline = millis() + 3000;
  while (millis() < deadline) {
    int size = udp.parsePacket();
    if (size > 0) {
      uint8_t buf[512];
      int len = udp.read(buf, sizeof(buf));
      // ID_UNCONNECTED_PONG = 0x1C
      if (len > 0 && buf[0] == 0x1C) {
        unsigned long recvTime = micros();
        pingMs = (int32_t)((recvTime - sendTime) / 1000);
        received = true;
        break;
      }
    }
    delay(10);
  }

  udp.stop();
  return received;
}

// ---------------------------------------------------------------------------
// Fetch public IPv4 from api.ipify.org via plain HTTP GET
// ---------------------------------------------------------------------------
bool fetchPublicIP(String& ip) {
  WiFiClient client;

  if (!client.connect(PUBLIC_IP_HOST, PUBLIC_IP_PORT)) {
    Serial.println("Could not connect to api.ipify.org");
    return false;
  }

  client.print("GET / HTTP/1.0\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n");

  // Wait for response (up to 5 seconds)
  uint32_t deadline = millis() + 5000;
  while (!client.available() && millis() < deadline) {
    delay(10);
  }

  // Read full response
  String response = "";
  while (client.available()) {
    response += (char)client.read();
  }
  client.stop();

  // Response body is after the blank line separating headers from body
  int bodyStart = response.indexOf("\r\n\r\n");
  if (bodyStart == -1) {
    Serial.println("Malformed HTTP response");
    return false;
  }

  String body = response.substring(bodyStart + 4);
  body.trim();

  if (body.length() == 0 || body.length() > 15) {
    Serial.printf("Unexpected IP response: %s\n", body.c_str());
    return false;
  }

  ip = body;
  return true;
}

// ---------------------------------------------------------------------------
// Draw the OLED layout
//
//  Line 0 (y=0):  WiFi: CONNECTED / DISCONNECTED
//  Line 1 (y=10): Local IP (small)
//  Divider (y=19)
//  Line 2 (y=21): MC SERVER
//  Line 3 (y=31): ONLINE / OFFLINE  (large if online)
//  Line 4 (y=43): Ping: XX ms  (only if online)
//  Divider (y=52)
//  Line 5 (y=54): Public IP (small)
// ---------------------------------------------------------------------------
void drawDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // --- WiFi status ---
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("WiFi: ");
  if (wifiConnected) {
    display.print("CONNECTED");
  } else {
    display.print("OFFLINE");
  }

  // Local IP
  display.setCursor(0, 10);
  if (wifiConnected) {
    display.print(WiFi.localIP().toString());
  } else {
    display.print("---.---.---.---");
  }

  // Divider
  display.drawFastHLine(0, 19, SCREEN_WIDTH, SSD1306_WHITE);

  // --- MC Server ---
  display.setTextSize(1);
  display.setCursor(0, 21);
  display.print("MC SERVER");

  if (mcOnline) {
    // "ONLINE" in large text
    display.setTextSize(2);
    display.setCursor(0, 31);
    display.print("ONLINE");

    display.setTextSize(1);
    display.setCursor(0, 47);
    display.print("Ping: ");
    display.print(mcPingMs);
    display.print(" ms");
  } else {
    display.setTextSize(2);
    display.setCursor(0, 31);
    display.print("OFFLINE");
  }

  // Divider
  display.drawFastHLine(0, 55, SCREEN_WIDTH, SSD1306_WHITE);

  // --- Public IP ---
  display.setTextSize(1);
  display.setCursor(0, 57);
  display.print(publicIP);

  display.display();
}

// ---------------------------------------------------------------------------
// Helper: draw text centered horizontally at given y
// ---------------------------------------------------------------------------
void drawCentered(const char* text, int16_t y, uint8_t size) {
  display.setTextSize(size);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, y);
  display.print(text);
}
