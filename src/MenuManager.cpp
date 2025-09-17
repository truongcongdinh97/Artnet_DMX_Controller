
#include "MenuManager.h"
#include <LiquidCrystal_I2C.h>
#include <AiEsp32RotaryEncoder.h>
#include "PlaybackController.h"
#include "DisplayUI.h"
#include "Recording.h"
#include "SDManager.h"
#include <Preferences.h>

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

void MenuManager::begin() {
    lcd.init();
    lcd.backlight();

    encoder.begin();
    encoder.setup(readEncoderISR);
    encoder.setBoundaries(0, 3, false); // 4 mục: Streaming, Recording, Playback, About
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

void MenuManager::loop() {
    if (encoder.encoderChanged()) {
        if (menuLevel == 0) {
            // menu chính: 0-Streaming, 1-Recording, 2-Playback, 3-About
            int val = encoder.readEncoder();
            if (val < 0) val = 0;
            if (val > 3) val = 3;
            encoder.setEncoderValue(val);

            // Nếu chọn About thì vào menu con About
            if (val == 3) {
                menuLevel = 2; // menu con About
                aboutMenuIndex = 0;
                encoder.setBoundaries(0, 1, false); // 0 = About Device, 1 = Reset
                encoder.setEncoderValue(aboutMenuIndex);
                lcd.clear();
                lcd.setCursor(0,0); lcd.print("About:");
                lcd.setCursor(0,1); lcd.print("About Device");
                lcd.setCursor(0,2); lcd.print("Reset");
                return;
            }

            // Kiểm tra SD khả dụng
            if ((val == 1 || val == 2) && !SDManager::isAvailable()) {
                currentMode = MODE_STREAMING;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Không tìm thấy SD!");
                lcd.setCursor(0, 1);
                lcd.print("Chỉ cho phép STREAM");
                delay(2000);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Chế độ: STREAMING");
                encoder.setEncoderValue(0);
            } else {
                switch (val) {
                    case 0: currentMode = MODE_STREAMING; break;
                    case 1: currentMode = MODE_RECORDING; break;
                    case 2: currentMode = MODE_PLAYBACK; break;
                }
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Mode: ");
                if (currentMode == MODE_STREAMING) lcd.print("STREAMING");
                else if (currentMode == MODE_RECORDING) lcd.print("RECORDING");
                else lcd.print("PLAYBACK");
            }
        }
        else if (menuLevel == 1 && currentMode == MODE_PLAYBACK) {
            // trong playback menu
            if (menuIndex == 0) {
                // chọn loop mode
                int val = encoder.readEncoder() % 3;
                loopMode = static_cast<PlaybackMode>(val);

                // gọi trực tiếp playback
                if (!playbackFiles.empty()) {
                    PlaybackController::setMode(loopMode);
                    PlaybackController::playFile(playbackFiles[selectedFileIndex]);
                }
            }
            else if (menuIndex == 1 && !playbackFiles.empty()) {
                // chọn file
                int maxIndex = playbackFiles.size() - 1;
                int val = encoder.readEncoder();
                if (val < 0) val = 0;
                if (val > maxIndex) val = maxIndex;
                selectedFileIndex = val;

                // gọi trực tiếp playback
                PlaybackController::setMode(loopMode);
                PlaybackController::playFile(playbackFiles[selectedFileIndex]);
                playbackActive = true;
                resetPlaybackMillis();
            }
        }
        else if (menuLevel == 1 && currentMode == MODE_RECORDING) {
            // Record menu: 0 = Start/Stop
            int val = encoder.readEncoder();
            if (val < 0) val = 0;
            if (val > 1) val = 1;
            encoder.setEncoderValue(val);
            if (val == 0 && !recordActive) {
                // Ready, nhấn nút để Start
            }
            if (val == 1 && recordActive) {
                // Đang ghi, nhấn nút để Stop
            }
        }
    }

    if (encoder.isEncoderButtonClicked()) {
        if (menuLevel == 0 && currentMode == MODE_PLAYBACK) {
            // vào playback menu
            menuLevel = 1;
            menuIndex = 0;
            encoder.setBoundaries(0, 2, false); // loop modes
            encoder.setEncoderValue(loopMode);
        }
        else if (menuLevel == 1) {
            if (currentMode == MODE_PLAYBACK) {
                if (menuIndex == 0) {
                    // chuyển sang chọn file
                    menuIndex = 1;
                    if (!playbackFiles.empty()) {
                        encoder.setBoundaries(0, playbackFiles.size() - 1, false);
                        encoder.setEncoderValue(selectedFileIndex);
                        // auto start playback khi vào chọn file
                        PlaybackController::setMode(loopMode);
                        PlaybackController::playFile(playbackFiles[selectedFileIndex]);
                        playbackActive = true;
                        resetPlaybackMillis();
                    }
                }
                else {
                    // thoát về menu chính
                    menuLevel = 0;
                    encoder.setBoundaries(0, 3, false);
                    encoder.setEncoderValue(static_cast<int>(currentMode));
                    playbackActive = false;
                }
            } else if (currentMode == MODE_RECORDING) {
                // Record: 0 = Start, 1 = Stop
                int val = encoder.readEncoder();
                if (val == 0 && !recordActive) {
                    MenuManager::startRecord();
                } else if (val == 1 && recordActive) {
                    MenuManager::stopRecord();
                }
            }
        }
        else if (menuLevel == 2) {
            // Menu con About
            aboutMenuIndex = encoder.readEncoder();
            if (aboutMenuIndex == 0) {
                // About Device: chỉ hiển thị thông tin
                lcd.clear();
                lcd.setCursor(0,0); lcd.print("LED Controller");
                lcd.setCursor(0,1); lcd.print("FW: 2025-09-17");
                lcd.setCursor(0,2); lcd.print("By: YourName");
                delay(2000);
                // Quay lại menu About
                lcd.clear();
                lcd.setCursor(0,0); lcd.print("About:");
                lcd.setCursor(0,1); lcd.print("About Device");
                lcd.setCursor(0,2); lcd.print("Reset");
            } else if (aboutMenuIndex == 1) {
                // RESET cấu hình WiFi
                lcd.clear();
                lcd.setCursor(0,0); lcd.print("Resetting...");
                Preferences prefs;
                prefs.begin("artnetcfg", false);
                prefs.clear();
                prefs.end();
                delay(1000);
                ESP.restart();
            }
        }
        else if (menuLevel == 0 && currentMode == MODE_RECORDING) {
            // vào record menu
            menuLevel = 1;
            menuIndex = 0;
            encoder.setBoundaries(0, 1, false); // 0 = Start, 1 = Stop
            encoder.setEncoderValue(recordActive ? 1 : 0);
        }
    }

    // Cập nhật LCD
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
