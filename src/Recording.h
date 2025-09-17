#pragma once
#include <Arduino.h>

namespace Recording {
  void begin();
  void loop();
  void start();
  void stop();
  void recordPacket(uint16_t universe, uint16_t length, const uint8_t* data);
  bool isRecording();
}
