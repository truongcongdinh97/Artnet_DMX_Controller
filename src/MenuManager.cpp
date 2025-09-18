#include "MenuManager.h"
#include <LiquidCrystal_I2C.h>
#include <AiEsp32RotaryEncoder.h>
#include "PlaybackController.h"
#include "DisplayUI.h"
#include "Recording.h"
#include "SDManager.h"
#include <Preferences.h>
#include <vector>

// LCD 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Rotary encoder
#include "PinConfig.h"
AiEsp32RotaryEncoder encoder(GPIO_ROTARY_A, GPIO_ROTARY_B, GPIO_ROTARY_BTN, -1);

static OperatingMode currentMode = MODE_STREAMING;

// Playback menu state
static std::vector<String> playbackFiles;
static int selectedFileIndex = 0;
static PlaybackMode loopMode = PLAY_ONCE;

// Navigation
// menuLevel: 0 = menu chính, 1 = menu con (Playback/Record/About)
static int menuLevel = 0;
static int menuIndex = 0;
static int aboutMenuIndex = 0; // menu con About

// Record state
static bool recordActive = false;
static uint32_t recordStartMillis = 0;
// Playback state
static uint32_t playbackStartMillis = 0;
static bool playbackActive = false;

static void IRAM_ATTR readEncoderISR() { encoder.readEncoder_ISR(); }

// Define main menu items
static const char* menuItems[] = { "Streaming", "Recording", "Playback", "Config" };
static const int menuCount = sizeof(menuItems)/sizeof(menuItems[0]);

void MenuManager::begin() {
    lcd.init();
    lcd.backlight();

    encoder.begin();
    encoder.setup(readEncoderISR);
    encoder.setBoundaries(0, menuCount - 1, false); // 0-Streaming,1-Recording,2-Playback,3-Config
    encoder.setAcceleration(0);

    if (!SDManager::isAvailable()) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("NO SD CARD FOUND");
        lcd.setCursor(0, 1);
        lcd.print("MODE: STREAMING");
        currentMode = MODE_STREAMING;
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mode: STREAMING");
    }
}

void MenuManager::setFileList(const std::vector<String>& files) {
    playbackFiles = files;
    selectedFileIndex = 0;
}

OperatingMode MenuManager::getMode() {
    return currentMode;
}

String MenuManager::getSelectedFile() {
    if (playbackFiles.empty()) return "";
    return playbackFiles[selectedFileIndex];
}

PlaybackMode MenuManager::getLoopMode() {
    return loopMode;
}

void MenuManager::startRecord() {
    if (!recordActive) {
        Recording::start();
        recordActive = true;
        recordStartMillis = millis();
    }
}
void MenuManager::stopRecord() {
    if (recordActive) {
        Recording::stop();
        recordActive = false;
    }
}
bool MenuManager::isRecording() { return recordActive; }
uint32_t MenuManager::getRecordMillis() { return recordActive ? (millis() - recordStartMillis) : 0; }

uint32_t MenuManager::getPlaybackMillis() { return playbackActive ? (millis() - playbackStartMillis) : 0; }
void MenuManager::resetPlaybackMillis() { playbackStartMillis = millis(); }

// New menu structure implementation
void MenuManager::loop() {
    if (encoder.encoderChanged()) {
        // Main menu selection
        int idx = encoder.readEncoder();
        if (idx < 0) idx = 0;
        if (idx >= menuCount) idx = menuCount - 1;
        menuIndex = idx;
        lcd.clear();
        lcd.setCursor(0,0); lcd.print("Menu:");
        lcd.setCursor(0,1); lcd.print(menuItems[menuIndex]);
    }

    if (encoder.isEncoderButtonClicked()) {
        // Enter selected mode
        currentMode = static_cast<OperatingMode>(menuIndex);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Mode: ");
        lcd.print(menuItems[menuIndex]);
    }

    if (menuLevel == 0) {
        lcd.setCursor(0, 1);
        lcd.print("                ");
    }
    else if (menuLevel == 1 && currentMode == MODE_PLAYBACK) {
        // Hiển thị trạng thái playback và thời gian
        DisplayUI::showPlaybackStatus(playbackFiles.empty() ? "" : playbackFiles[selectedFileIndex], getPlaybackMillis());
    }
    else if (menuLevel == 1 && currentMode == MODE_RECORDING) {
        // Hiển thị trạng thái record và thời gian
        DisplayUI::showRecordStatus(recordActive, recordActive ? (millis() - recordStartMillis) : 0);
    }
}
