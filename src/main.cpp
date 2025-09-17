#include <Arduino.h>
#include "ConfigManager.h"
#include "ArtnetHandler.h"
#include "LEDController.h"
#include "SDManager.h"
#include "WebUI.h"
#include "MenuManager.h"
#include "PlaybackController.h"
#include "Recording.h"


// Track last mode to detect mode changes
static int lastMode = -1;

void setup() {
    Serial.begin(115200);
    Serial.println("System starting...");

    ConfigManager::begin();
    LEDController::begin();
    SDManager::begin();
    WebUI::begin();
    Recording::begin();

    // Load danh sách file từ SD cho playback
    auto files = SDManager::listFiles("/");  
    PlaybackController::begin();
    PlaybackController::setFiles(files);

    MenuManager::begin();
    lastMode = MenuManager::getMode();
}

void loop() {
    MenuManager::loop();

    int mode = MenuManager::getMode();
    // Detect mode change
    if (mode != lastMode) {
        if (mode == MODE_RECORDING) {
            Recording::start();
            ArtnetHandler::setDmxPacketCallback(Recording::recordPacket);
        } else if (lastMode == MODE_RECORDING) {
            Recording::stop();
            ArtnetHandler::setDmxPacketCallback(nullptr);
        }
        lastMode = mode;
    }

    switch (mode) {
        case MODE_STREAMING:
            ArtnetHandler::loop();
            break;

        case MODE_RECORDING:
            // Process ArtNet packets and record them
            ArtnetHandler::loop();
            break;

        case MODE_PLAYBACK:
            PlaybackController::loop();
            break;
    }

    WebUI::loop();
}
