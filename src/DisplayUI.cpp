#include "DisplayUI.h"
#include <LiquidCrystal_I2C.h>
#include <AiEsp32RotaryEncoder.h>

#include "PinConfig.h"

static LiquidCrystal_I2C lcd(0x27, 20, 4);
static AiEsp32RotaryEncoder rotary(GPIO_ROTARY_A, GPIO_ROTARY_B, GPIO_ROTARY_BTN, -1, 4);

static Mode currentMode = STREAMING;
static int menuIndex = 0;
static const char* menuItems[] = { "Streaming", "Recording", "Playback", "Config" };
static const int menuCount = sizeof(menuItems)/sizeof(menuItems[0]);

static void IRAM_ATTR readEncoderISR() { rotary.readEncoder_ISR(); }

void DisplayUI::begin() {
  lcd.init();
  lcd.backlight();
  rotary.begin();
  rotary.setup(readEncoderISR);
  rotary.setBoundaries(0, menuCount - 1, true);
}

void DisplayUI::loop() {
  if (rotary.encoderChanged()) {
    menuIndex = rotary.readEncoder();
    if (menuIndex < 0) menuIndex = 0;
    if (menuIndex >= menuCount) menuIndex = menuCount - 1;
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Menu:");
    lcd.setCursor(0,1); lcd.print(menuItems[menuIndex]);
  }
  if (rotary.isEncoderButtonClicked()) {
    currentMode = (Mode)menuIndex;
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Mode: ");
    lcd.print(menuItems[menuIndex]);
  }
}

Mode DisplayUI::getMode() { return currentMode; }

void DisplayUI::showRecordStatus(bool isRecording, uint32_t recordMillis) {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("RECORD MODE");
  lcd.setCursor(0,1); lcd.print(isRecording ? "Recording..." : "Ready");
  lcd.setCursor(0,2); lcd.print("Time: ");
  uint32_t sec = recordMillis / 1000;
  uint8_t mm = sec / 60;
  uint8_t ss = sec % 60;
  if (mm < 10) lcd.print('0'); lcd.print(mm); lcd.print(":");
  if (ss < 10) lcd.print('0'); lcd.print(ss);
}

void DisplayUI::showPlaybackStatus(const String& filename, uint32_t playMillis) {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("PLAYBACK");
  lcd.setCursor(0,1); lcd.print("File: ");
  lcd.print(filename.length() > 12 ? filename.substring(0,12) : filename);
  lcd.setCursor(0,2); lcd.print("Time: ");
  uint32_t sec = playMillis / 1000;
  uint8_t mm = sec / 60;
  uint8_t ss = sec % 60;
  if (mm < 10) lcd.print('0'); lcd.print(mm); lcd.print(":");
  if (ss < 10) lcd.print('0'); lcd.print(ss);
}
