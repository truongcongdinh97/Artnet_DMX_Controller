#pragma once
#include <Arduino.h>

struct Config {
  String wifiSSID;
  String wifiPass;
  IPAddress localIP;
  int outputs;
  int ledsPerOutput;
  // Tên thiết bị cố định
  static constexpr const char* deviceName = "LED_Controller";
};

namespace ConfigManager {
  void begin();
  void saveConfig(const Config& cfg);
  Config getConfig();
}
