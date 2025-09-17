#include "ArtnetHandler.h"
#include "LEDController.h"
#include <Arduino.h>

static UDP* udp = nullptr;

static const uint16_t ARTNET_PORT = 6454;
static uint8_t packetBuf[600]; // Art-Net tối đa 530 + chút dư
static ArtnetHandler::DmxPacketCallback dmxCallback = nullptr;

void ArtnetHandler::setDmxPacketCallback(DmxPacketCallback cb) {
  dmxCallback = cb;
}

void ArtnetHandler::begin(UDP* udp_instance) {
  udp = udp_instance;
  if (udp) {
    Serial.println("[ArtnetHandler] Started.");
  } else {
    Serial.println("[ArtnetHandler] Error: UDP instance is null.");
  }
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
  if (len < 18 || memcmp(packetBuf, artHeader, 7) != 0 || packetBuf[7] != 0) {
    return; // Không phải gói Art-Net
  }

  // opcode (little endian)
  uint16_t opcode = packetBuf[8] | (packetBuf[9] << 8);
  const uint16_t OpDmx = 0x5000;
  if (opcode != OpDmx) return;

  uint16_t universe = packetBuf[14] | (packetBuf[15] << 8); // little endian
  uint16_t dataLen = (packetBuf[16] << 8) | packetBuf[17]; // big endian
  if (dataLen == 0 || dataLen > (len - 18)) dataLen = len - 18;
  uint8_t* dataPtr = packetBuf + 18;

  // Gửi dữ liệu đến LEDController
  LEDController::updateFromArtnet(universe, dataLen, dataPtr);
  
  // Gọi callback nếu đã được set (dùng cho việc ghi file)
  if (dmxCallback) {
    dmxCallback(universe, dataLen, dataPtr);
  }
}