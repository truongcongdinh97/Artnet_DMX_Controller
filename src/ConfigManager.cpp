#include "ConfigManager.h"
#include <Preferences.h>

static Preferences prefs;
static Config config;

void ConfigManager::begin() {
  prefs.begin("artnetcfg", false);

  config.wifiSSID = prefs.getString("ssid", "ESP32");
  config.wifiPass = prefs.getString("pass", "12345678");
  config.localIP = IPAddress(192,168,1,50);
  uint32_t ip = prefs.getUInt("localIP", 0);
  config.localIP.fromString(prefs.getString("localIP", "192.168.1.50"));

  config.outputs = prefs.getInt("outputs", 4);
  config.ledsPerOutput = prefs.getInt("leds", 680);
  config.startUniverse = prefs.getInt("startUni", 0);

  Serial.println("Config loaded.");
}

void ConfigManager::saveConfig(const Config& cfg) {
  prefs.putString("ssid", cfg.wifiSSID);
  prefs.putString("pass", cfg.wifiPass);
  prefs.putString("localIP", cfg.localIP.toString());
  prefs.putInt("outputs", cfg.outputs);
  prefs.putInt("leds", cfg.ledsPerOutput);
  prefs.putInt("startUni", cfg.startUniverse);
}

Config ConfigManager::getConfig() {
  return config;
}

