#include "ArtnetHandler.h"
#include "PinConfig.h"
#include <Arduino.h>
#include <IPAddress.h>
#include "SendArtPollReply.h"  // add at top
#include <HardwareSerial.h>

// Art-Net opcodes
static const uint16_t OpPoll      = 0x2000;
static const uint16_t OpPollReply = 0x2100;

static UDP* udp = nullptr;

static const uint16_t ARTNET_PORT = 6454;
static uint8_t packetBuf[600]; // Art-Net tối đa 530 + chút dư
static ArtnetHandler::DmxPacketCallback dmxCallback = nullptr;

// RS485 DMX outputs
static HardwareSerial DMX1(2); // UART2 for Universe 1
static HardwareSerial DMX2(1); // UART1 for Universe 2

void ArtnetHandler::setDmxPacketCallback(DmxPacketCallback cb) {
  dmxCallback = cb;
}

// Initialize RS485 DE pins and UARTs
void setupDmx() {
  pinMode(GPIO_RS485_1_DE, OUTPUT);
  pinMode(GPIO_RS485_2_DE, OUTPUT);
  digitalWrite(GPIO_RS485_1_DE, LOW);
  digitalWrite(GPIO_RS485_2_DE, LOW);
  DMX1.begin(250000, SERIAL_8N2, -1, GPIO_RS485_1_TX);
  DMX2.begin(250000, SERIAL_8N2, -1, GPIO_RS485_2_TX);
}

void ArtnetHandler::begin(UDP* udp_instance) {
  udp = udp_instance;
  if (udp) {
    Serial.println("[ArtnetHandler] Started.");
  } else {
    Serial.println("[ArtnetHandler] Error: UDP instance is null.");
  }
  setupDmx();
}

// Send DMX packet out on RS485
static void sendDmx(uint16_t universe, const uint8_t* data, uint16_t length) {
  HardwareSerial* serial = nullptr;
  uint8_t dePin = 0;
  if (universe == 0) {
    serial = &DMX1; dePin = GPIO_RS485_1_DE;
  } else if (universe == 1) {
    serial = &DMX2; dePin = GPIO_RS485_2_DE;
  } else {
    return; // unsupported universe
  }
  // Enable driver
  digitalWrite(dePin, HIGH);
  // Send DMX data (start code 0)
  serial->write((uint8_t)0);
  serial->write(data, length);
  serial->flush();
  // Disable driver
  digitalWrite(dePin, LOW);
}

void ArtnetHandler::loop() {
  if (udp == nullptr) return;

  int packetSize = udp->parsePacket();
  if (packetSize <= 0) return;
  if (packetSize > (int)sizeof(packetBuf)) packetSize = sizeof(packetBuf);

  int len = udp->read(packetBuf, packetSize);
  if (len <= 0) return;

  // Minimal Art-Net header check (allow Poll: len>=14)
  const char artHeader[] = "Art-Net";
  if (len < 14 || memcmp(packetBuf, artHeader, 7) != 0 || packetBuf[7] != 0) {
    return; // Not Art-Net packet
  }

  // opcode (little endian)
  uint16_t opcode = packetBuf[8] | (packetBuf[9] << 8);
  // Debug: log opcode of incoming packet
  Serial.printf("[ArtnetHandler] Packet received: size=%d, opcode=0x%04X\n", packetSize, opcode);

  // handle ArtPoll
  if (opcode == OpPoll) {
    sendArtPollReply(udp);
    return;
  }

  const uint16_t OpDmx = 0x5000;
  if (opcode != OpDmx) return;
  // Ensure DMX packet has full header (18 bytes)
  if (len < 18) return;

  // Extract universe and data length
  uint16_t universe = packetBuf[14] | (packetBuf[15] << 8);
  uint16_t dataLen = (packetBuf[16] << 8) | packetBuf[17];
  if (dataLen == 0 || dataLen > (len - 18)) dataLen = len - 18;
  uint8_t* dataPtr = packetBuf + 18;

  // Debug: log received packet
  Serial.printf("[ArtnetHandler] Received DMX packet: Universe=%u, Length=%u\n", universe, dataLen);

  // Replace LEDController with RS485 send
  sendDmx(universe - 1, dataPtr, dataLen); // universe index 0 or 1
  
  // Gọi callback nếu đã được set (dùng cho việc ghi file)
  if (dmxCallback) {
    dmxCallback(universe, dataLen, dataPtr);
  }
}