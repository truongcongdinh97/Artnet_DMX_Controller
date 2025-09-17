#pragma once
#include <Arduino.h>

namespace LEDController {
  void begin();
  void loop();
  void updateFromArtnet(uint16_t universe, uint16_t length, uint8_t* data);
}
