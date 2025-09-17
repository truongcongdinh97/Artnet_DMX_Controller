#pragma once
#include <Arduino.h>

namespace WebUI {
    // Khởi động web UI
    void begin();

    // Hàm loop gọi trong main.cpp
    void loop();

    // Chế độ Access Point (AP Mode)
    void startAP(const char* ssid = "ESP32-AP", const char* password = "12345678");

    // Chế độ Station (kết nối WiFi sẵn có)
    void startSTA(const char* ssid = nullptr, const char* password = nullptr);

    // API cấu hình (GET/POST)
    void setupConfigApi();

    // API OTA update
    void setupOTA();
}
