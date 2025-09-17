#include "ArtnetHandler.h"
#include "LEDController.h"
#include "WebUI.h"       // để gọi startAP / startSTA nếu cần
#include <WiFi.h>
#include <Ethernet.h>
#include <WiFiUdp.h>
#include <EthernetUdp.h>

static WiFiUDP wifiUdp;
static EthernetUDP ethUdp;
static UDP* udp = nullptr;

static bool useEthernet = false;
static bool useWiFi = false;


static const uint16_t ARTNET_PORT = 6454;
static uint8_t packetBuf[600]; // Art-Net tối đa 530 + chút dư
static ArtnetHandler::DmxPacketCallback dmxCallback = nullptr;
void ArtnetHandler::setDmxPacketCallback(DmxPacketCallback cb) {
  dmxCallback = cb;
}

// Trả về true nếu Ethernet được link (có cắm cable)
static bool tryEthernetInit() {
  byte mac[6] = { 0xDE,0xAD,0xBE,0xEF,0xFE,0xED };
  // Nếu W5500: Ethernet.begin(mac) trả về IP nếu DHCP ok, hoặc 0 nếu fail.
  IPAddress ip;
  if (Ethernet.begin(mac) == 0) {
    // DHCP failed; nhưng linkStatus có thể cho biết
    if (Ethernet.linkStatus() == LinkON) {
      // Try static fallback if needed (could set IP later)
      // but return true for link present
      return true;
    }
    return false;
  }
  // DHCP ok
  if (Ethernet.linkStatus() == LinkON) {
    return true;
  }
  return false;
}

void ArtnetHandler::begin(const Config& cfg) {
  useEthernet = false;
  useWiFi = false;
  udp = nullptr;

  Serial.println("[ArtnetHandler] Trying Ethernet...");
  if (tryEthernetInit()) {
    // Start UDP on Ethernet
    if (ethUdp.begin(ARTNET_PORT) == 1) {
      udp = &ethUdp;
      useEthernet = true;
      Serial.print("[ArtnetHandler] Ethernet active, IP: ");
      Serial.println(Ethernet.localIP());
      // Start WebUI in STA mode served by Ethernet if you want
      WebUI::startSTA(); // optional: implement to bind to network mode
      return;
    } else {
      Serial.println("[ArtnetHandler] Ethernet UDP begin failed.");
    }
  } else {
    Serial.println("[ArtnetHandler] Ethernet not present or no link.");
  }

  // --- Fallback to WiFi ---
  Serial.println("[ArtnetHandler] Starting WiFi client...");
  if (cfg.wifiSSID.length() > 0) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(cfg.wifiSSID.c_str(), cfg.wifiPass.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
      delay(200);
      Serial.print(".");
    }
    Serial.println();
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (wifiUdp.begin(ARTNET_PORT) == 1) {
      udp = &wifiUdp;
      useWiFi = true;
      Serial.print("[ArtnetHandler] WiFi active, IP: ");
      Serial.println(WiFi.localIP());
      WebUI::startSTA();
      return;
    } else {
      Serial.println("[ArtnetHandler] WiFi UDP begin failed.");
    }
  }

  // --- If reached here, neither Ethernet nor WiFi STA is ready: start AP for configuration ---
  Serial.println("[ArtnetHandler] No network, starting AP mode.");
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Artnet-ESP32", "12345678"); // change later via WebUI
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("[ArtnetHandler] AP IP: "); Serial.println(apIP);

  // Start UDP even in AP (listens on WiFi AP interface)
  if (wifiUdp.begin(ARTNET_PORT) == 1) {
    udp = &wifiUdp;
    useWiFi = true;
  }

  WebUI::startAP(); // implement to show config page
}

void ArtnetHandler::loop() {
  if (udp == nullptr) return;

  int packetSize = udp->parsePacket();
  if (packetSize <= 0) return;
  if (packetSize > (int)sizeof(packetBuf)) packetSize = sizeof(packetBuf);

  int len = udp->read(packetBuf, packetSize);
  if (len <= 0) return;

  // Minimal Art-Net header check
  const char artHeader[] = "Art-Net";
  if (len < 18) return; // too small
  if (memcmp(packetBuf, artHeader, 7) != 0 || packetBuf[7] != 0) return;

  // opcode (little endian)
  uint16_t opcode = packetBuf[8] | (packetBuf[9] << 8);
  const uint16_t OpDmx = 0x5000;
  if (opcode != OpDmx) return;

  // protocol version is at 10-11 (big endian) - we can ignore
  // sequence: 12, physical:13
  uint16_t universe = packetBuf[14] | (packetBuf[15] << 8); // little endian
  // length (big endian) at 16-17
  uint16_t dataLen = (packetBuf[16] << 8) | packetBuf[17];
  if (dataLen == 0 || dataLen > (len - 18)) dataLen = len - 18;
  uint8_t* dataPtr = packetBuf + 18;

  // Pass to LEDController
  LEDController::updateFromArtnet(universe, dataLen, dataPtr);
  // If a callback is set (e.g., for recording), call it
  if (dmxCallback) {
    dmxCallback(universe, dataLen, dataPtr);
  }
}

bool ArtnetHandler::isEthernetActive() { return useEthernet; }
bool ArtnetHandler::isWifiActive() { return useWiFi; }
