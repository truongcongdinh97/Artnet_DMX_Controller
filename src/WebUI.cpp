#include "WebUI.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Update.h>
#include "ConfigManager.h"

// Web server chạy cổng 80
static AsyncWebServer server(80);

void WebUI::begin() {
    // Khởi động SPIFFS nếu chưa chạy
    if (!SPIFFS.begin(true)) {
        Serial.println("[WebUI] SPIFFS Mount Failed");
    }

    // Route mặc định chuyển về giao diện cấu hình
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->redirect("/config.html");
    });

    // Phục vụ file HTML giao diện cấu hình
    server.serveStatic("/config.html", SPIFFS, "/config.html");

    // Ví dụ thêm 1 API
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
        String msg = "ESP32 running. IP: " + WiFi.localIP().toString();
        request->send(200, "text/plain", msg);
    });

    // API lấy cấu hình hiện tại
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
        Config cfg = ConfigManager::getConfig();
        String json = "{";
        json += "\"wifiSSID\":\"" + cfg.wifiSSID + "\",";
        json += "\"wifiPass\":\"" + cfg.wifiPass + "\",";
        json += "\"localIP\":\"" + cfg.localIP.toString() + "\",";
        json += "\"outputs\":" + String(cfg.outputs) + ",";
        json += "\"ledsPerOutput\":" + String(cfg.ledsPerOutput) + ",";
        json += "\"startUniverse\":" + String(cfg.startUniverse) + ",";
        json += "\"deviceName\":\"LED_Controller\"";
        json += "}";
        request->send(200, "application/json", json);
    });

    // API cập nhật cấu hình (POST)
    server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request){
        Config cfg = ConfigManager::getConfig();
        if (request->hasParam("wifiSSID", true)) cfg.wifiSSID = request->getParam("wifiSSID", true)->value();
        if (request->hasParam("wifiPass", true)) cfg.wifiPass = request->getParam("wifiPass", true)->value();
        if (request->hasParam("localIP", true)) cfg.localIP.fromString(request->getParam("localIP", true)->value());
        if (request->hasParam("outputs", true)) cfg.outputs = request->getParam("outputs", true)->value().toInt();
        if (request->hasParam("ledsPerOutput", true)) cfg.ledsPerOutput = request->getParam("ledsPerOutput", true)->value().toInt();
        if (request->hasParam("startUniverse", true)) cfg.startUniverse = request->getParam("startUniverse", true)->value().toInt();
        // Không cho đổi deviceName qua WebUI
        ConfigManager::saveConfig(cfg);
        request->send(200, "text/plain", "Config updated. Please reboot to apply changes.");
    });

    server.begin();
}

void WebUI::loop() {
    // ESPAsyncWebServer không cần xử lý trong loop
    // Nếu sau này cần auto reconnect WiFi thì thêm vào đây
}

void WebUI::startAP(const char* ssid, const char* password) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    Serial.println("[WebUI] Started Access Point");
    Serial.print("SSID: "); Serial.println(ssid);
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
}

void WebUI::startSTA(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("[WebUI] Connecting to "); Serial.println(ssid);
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 20) {
        delay(500);
        Serial.print(".");
        retry++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WebUI] WiFi connected");
        Serial.print("IP: "); Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n[WebUI] WiFi connect failed, switching to AP mode");
        startAP(); // fallback
    }
}
