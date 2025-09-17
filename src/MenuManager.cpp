#include "MenuManager.h"
#include <LiquidCrystal_I2C.h>
#include <AiEsp32RotaryEncoder.h>
#include "PlaybackController.h"

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
static int menuLevel = 0;   // 0 = mode select, 1 = playback control
static int menuIndex = 0;

static void IRAM_ATTR readEncoderISR() { encoder.readEncoder_ISR(); }

void MenuManager::begin() {
    lcd.init();
    lcd.backlight();

    encoder.begin();
    encoder.setup(readEncoderISR);
    encoder.setBoundaries(0, 2, false); // 3 chế độ
    encoder.setAcceleration(0);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mode: STREAMING");
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

void MenuManager::loop() {
    if (encoder.encoderChanged()) {
        if (menuLevel == 0) {
            // chọn mode
            int val = encoder.readEncoder();
            if (val < 0) val = 0;
            if (val > 2) val = 2;
            encoder.setEncoderValue(val);

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
            if (menuIndex == 0) {
                // chuyển sang chọn file
                menuIndex = 1;
                if (!playbackFiles.empty()) {
                    encoder.setBoundaries(0, playbackFiles.size() - 1, false);
                    encoder.setEncoderValue(selectedFileIndex);

                    // auto start playback khi vào chọn file
                     PlaybackController::setMode(loopMode);
                     PlaybackController::playFile(playbackFiles[selectedFileIndex]);
                }
            }
            else {
                // thoát về menu chính
                menuLevel = 0;
                encoder.setBoundaries(0, 2, false);
                encoder.setEncoderValue(static_cast<int>(currentMode));
            }
        }
    }

    // Cập nhật LCD
    if (menuLevel == 0) {
        lcd.setCursor(0, 1);
        lcd.print("                ");
    }
    else if (menuLevel == 1 && currentMode == MODE_PLAYBACK) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Playback Menu");

        lcd.setCursor(0, 1);
        lcd.print("Loop: ");
    if (loopMode == PLAY_ONCE) lcd.print("Once ");
        else if (loopMode == LOOP_ONE) lcd.print("One  ");
        else lcd.print("All  ");

        lcd.setCursor(0, 2);
        lcd.print("File: ");
        if (!playbackFiles.empty()) {
            lcd.print(playbackFiles[selectedFileIndex]);
        } else {
            lcd.print("No files");
        }
    }
}
