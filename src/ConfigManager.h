#pragma once
#include <Arduino.h>

struct Config {
  String wifiSSID;
  String wifiPass;
  IPAddress localIP;
  int outputs;
  int ledsPerOutput;
};

namespace ConfigManager {
  void begin();
  void saveConfig(const Config& cfg);
  Config getConfig();
}
