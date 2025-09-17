#include "WebUI.h"
#include "ConfigManager.h"
#include <WiFi.h>
#include <Ethernet.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include "PinConfig.h"

// Network instances
static EthernetUDP ethUdp;
static WiFiUDP wifiUdp;
static UDP* activeUdp = nullptr;
static const uint16_t ARTNET_PORT = 6454;

// Web server
static AsyncWebServer server(80);

// Hàm private để khởi tạo mạng
static void initialize_network() {
    // 1. Thử Ethernet trước
    byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    Serial.println("[WebUI] Trying Ethernet...");
    // Lưu ý: Đảm bảo GPIO_ETH_CS được định nghĩa đúng trong PinConfig.h
    // Ethernet.init(GPIO_ETH_CS); 
    if (Ethernet.begin(mac, 10000)) { // Thử DHCP trong 10s
        if (Ethernet.linkStatus() == LinkON) {
            Serial.print("[WebUI] Ethernet connected. IP: ");
            Serial.println(Ethernet.localIP());
            if (ethUdp.begin(ARTNET_PORT)) {
                activeUdp = &ethUdp;
                Serial.println("[WebUI] Ethernet UDP listener started.");
                return; // Ưu tiên Ethernet thành công
            }
        }
    }
    Serial.println("[WebUI] Ethernet failed or not present.");

    // 2. Nếu Ethernet thất bại, thử WiFi STA
    Config cfg = ConfigManager::getConfig();
    if (cfg.wifiSSID.length() > 0 && cfg.wifiSSID != "ESP32") {
        Serial.print("[WebUI] Connecting to WiFi: ");
        Serial.println(cfg.wifiSSID);
        WiFi.mode(WIFI_STA);
        WiFi.begin(cfg.wifiSSID.c_str(), cfg.wifiPass.c_str());
        
        uint8_t timeout = 20; // 10 giây
        while (WiFi.status() != WL_CONNECTED && timeout > 0) {
            Serial.print(".");
            delay(500);
            timeout--;
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("[WebUI] WiFi connected. IP: ");
            Serial.println(WiFi.localIP());
            if (wifiUdp.begin(ARTNET_PORT)) {
                activeUdp = &wifiUdp;
                Serial.println("[WebUI] WiFi UDP listener started.");
                return; // WiFi STA thành công
            }
        }
        Serial.println("[WebUI] WiFi connection failed.");
    }

    // 3. Nếu tất cả thất bại, phát AP
    Serial.println("[WebUI] Starting Access Point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("LED_Controller-Setup", "12345678");
    Serial.print("[WebUI] AP SSID: LED_Controller-Setup, IP: ");
    Serial.println(WiFi.softAPIP());
    if (wifiUdp.begin(ARTNET_PORT)) {
        activeUdp = &wifiUdp;
        Serial.println("[WebUI] WiFi AP UDP listener started.");
    }
}

void WebUI::begin() {
    // Khởi tạo mạng trước
    initialize_network();

    // Khởi động SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("[WebUI] SPIFFS Mount Failed");
    }

    // Cấu hình các route cho web server
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->redirect("/config.html");
    });
    server.serveStatic("/", SPIFFS, "/");

    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
        Config cfg = ConfigManager::getConfig();
        String json = "{";
        json += "\"wifiSSID\":\"" + cfg.wifiSSID + "\",";
        json += "\"localIP\":\"" + cfg.localIP.toString() + "\",";
        json += "\"outputs\":" + String(cfg.outputs) + ",";
        json += "\"ledsPerOutput\":" + String(cfg.ledsPerOutput) + ",";
        json += "\"startUniverse\":" + String(cfg.startUniverse);
        json += "}";
        request->send(200, "application/json", json);
    });

    server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
        Config cfg = ConfigManager::getConfig();
        if (request->hasParam("wifiSSID", true)) cfg.wifiSSID = request->getParam("wifiSSID", true)->value();
        if (request->hasParam("wifiPass", true)) cfg.wifiPass = request->getParam("wifiPass", true)->value();
        if (request->hasParam("localIP", true)) cfg.localIP.fromString(request->getParam("localIP", true)->value());
        if (request->hasParam("outputs", true)) cfg.outputs = request->getParam("outputs", true)->value().toInt();
        if (request->hasParam("ledsPerOutput", true)) cfg.ledsPerOutput = request->getParam("ledsPerOutput", true)->value().toInt();
        if (request->hasParam("startUniverse", true)) cfg.startUniverse = request->getParam("startUniverse", true)->value().toInt();
        
        ConfigManager::saveConfig(cfg);
        String response = "Config saved. Rebooting...";
        request->send(200, "text/plain", response);
        delay(1000);
        ESP.restart();
    });

    // Khởi động server
    server.begin();
    Serial.println("[WebUI] Web server started.");
}

void WebUI::loop() {
    // Không cần làm gì ở đây
}

UDP* WebUI::getUDP() {
    return activeUdp;
}
