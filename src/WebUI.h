#pragma once
#include <Arduino.h>
#include <Udp.h>

namespace WebUI {
    // Khởi động toàn bộ hệ thống mạng và web UI
    void begin();

    // Hàm loop (có thể không cần dùng với ESPAsyncWebServer)
    void loop();

    // Trả về đối tượng UDP đang hoạt động (Ethernet hoặc WiFi)
    UDP* getUDP();
}
