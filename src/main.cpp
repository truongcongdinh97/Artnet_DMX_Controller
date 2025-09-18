#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "ArtnetHandler.h"
#include "ConfigManager.h"
#include "SDManager.h"
#include "MenuManager.h"
#include "WebUI.h"
#include "PlaybackController.h"
#include "Recording.h"

static WiFiUDP udp;
static OperatingMode lastMode;

void setup() {
    Serial.begin(115200);
    Serial.println("\nSystem starting...");

    // 1. Core modules
    ConfigManager::begin();
    // No LEDController - DMX via RS485

    // 2. SD card
    SDManager::begin();
    if (SDManager::isAvailable()) {
        auto files = SDManager::listFiles("/");
        MenuManager::setFileList(files);
        PlaybackController::setFiles(files);
    }
    Recording::begin();
    PlaybackController::begin();

    // 3. Network & Web UI
    WebUI::begin();

    // 4. Art-Net handler using UDP
    ArtnetHandler::begin(&udp);

    // 5. Menu & LCD
    MenuManager::begin();
    lastMode = MenuManager::getMode();
}

void loop() {
    MenuManager::loop();
    WebUI::loop();

    auto currentMode = MenuManager::getMode();

    if (currentMode == MODE_STREAMING || currentMode == MODE_RECORDING) {
        ArtnetHandler::loop();
    }

    if (currentMode == MODE_PLAYBACK) {
        PlaybackController::loop();
    }

    if (currentMode != lastMode) {
        if (currentMode == MODE_RECORDING) {
            // Recording uses ArtnetHandler callback
            ArtnetHandler::setDmxPacketCallback([](uint16_t universe, uint16_t length, const uint8_t* data) {
                Recording::recordPacket(universe, length, data);
            });
        }
        if (currentMode != MODE_PLAYBACK && lastMode == MODE_PLAYBACK) {
            PlaybackController::stop();
        }
        lastMode = currentMode;
    }
}
