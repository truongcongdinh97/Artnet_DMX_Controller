#pragma once
#include <Arduino.h>

struct Config {
  String wifiSSID;
  String wifiPass;
  IPAddress localIP;
  int outputs;
  int ledsPerOutput;
  int startUniverse; // Universe bắt đầu cho output 1
  // Tên thiết bị cố định
  static constexpr const char* deviceName = "DMX_Controller_2 Universes";

  Config();  // default constructor declaration
};

namespace ConfigManager {
  void begin();
  void saveConfig(const Config& cfg);
  Config getConfig();
}
