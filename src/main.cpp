#include <Arduino.h>
#include "ConfigManager.h"
#include "LEDController.h"
#include "SDManager.h"
#include "MenuManager.h"
#include "WebUI.h"
#include "ArtnetHandler.h"
#include "PlaybackController.h"
#include "Recording.h"


static OperatingMode lastMode;

void setup() {
    Serial.begin(115200);
    Serial.println("\nSystem starting...");

    // 1. Khởi tạo các module cơ bản
    ConfigManager::begin();
    LEDController::begin();
    
    // 2. Khởi tạo SD card và các module phụ thuộc
    SDManager::begin();
    if (SDManager::isAvailable()) {
        auto files = SDManager::listFiles("/");
        MenuManager::setFileList(files);
        PlaybackController::setFiles(files);
    }
    
    Recording::begin();
    PlaybackController::begin();

    // 3. Khởi tạo Mạng và WebUI (luôn chạy)
    WebUI::begin();

    // 4. Khởi tạo ArtnetHandler với UDP đã được WebUI chuẩn bị
    ArtnetHandler::begin(WebUI::getUDP());

    // 5. Khởi tạo Menu và LCD
    MenuManager::begin();
    lastMode = MenuManager::getMode();
}

void loop() {
    // Luôn chạy các module nền
    MenuManager::loop();
    WebUI::loop();

    OperatingMode currentMode = MenuManager::getMode();

    // Xử lý logic theo từng mode
    switch (currentMode) {
        case MODE_STREAMING:
            ArtnetHandler::loop();
            break;
        case MODE_RECORDING:
            ArtnetHandler::loop(); // Cần nhận Art-Net để ghi
            break;
        case MODE_PLAYBACK:
            PlaybackController::loop();
            break;
    }

    // Xử lý khi chuyển mode
    if (currentMode != lastMode) {
        if (currentMode == MODE_RECORDING) {
            ArtnetHandler::setDmxPacketCallback(Recording::recordPacket);
        } else if (lastMode == MODE_RECORDING) {
            ArtnetHandler::setDmxPacketCallback(nullptr);
        }
        
        if (currentMode != MODE_PLAYBACK && lastMode == MODE_PLAYBACK) {
            PlaybackController::stop();
        }
        
        lastMode = currentMode;
    }
}
