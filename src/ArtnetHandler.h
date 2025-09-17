#pragma once
#include <Arduino.h>
#include "ConfigManager.h"

namespace ArtnetHandler {
  typedef void (*DmxPacketCallback)(uint16_t universe, uint16_t length, const uint8_t* data);
  void begin(const Config& cfg);
  void loop();
  void setDmxPacketCallback(DmxPacketCallback cb);
  bool isEthernetActive();
  bool isWifiActive();
}
