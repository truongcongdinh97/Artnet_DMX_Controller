
#pragma once
#include <Arduino.h>
#include <vector>

namespace SDManager {
  void begin();
  void loop();
  // Recording logic moved to Recording module
  void startPlayback(const char* filename);
  void stopPlayback();
  std::vector<String> listFiles(const char* path = "/");
}
