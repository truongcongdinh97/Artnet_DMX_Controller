#pragma once
#include <Arduino.h>

namespace StatusLED {
  void begin(uint8_t pin = 2); // Mặc định dùng chân 2
  void setStreaming();   // Xanh dương
  void setRecording();   // Đỏ nhấp nháy
  void setPlayback();    // Xanh lá
  void loop();           // Gọi trong loop để nhấp nháy
}
