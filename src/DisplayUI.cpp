#include "DisplayUI.h"
#include <LiquidCrystal_I2C.h>
#include <AiEsp32RotaryEncoder.h>

#include "PinConfig.h"

static LiquidCrystal_I2C lcd(0x27, 20, 4);
static AiEsp32RotaryEncoder rotary(GPIO_ROTARY_A, GPIO_ROTARY_B, GPIO_ROTARY_BTN, -1, 4);

static Mode currentMode = STREAMING;
static int menuIndex = 0;
const char* menuItems[] = { "Mode", "Outputs", "LEDs", "Playback", "Brightness" };
const int menuCount = sizeof(menuItems)/sizeof(menuItems[0]);

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
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Menu:");
    lcd.setCursor(0,1); lcd.print(menuItems[menuIndex]);
  }
  if (rotary.isEncoderButtonClicked()) {
    if (menuIndex == 0) {
      currentMode = (Mode)((currentMode + 1) % 3);
      lcd.setCursor(0,2); lcd.print("Mode:");
      lcd.print(currentMode);
    }
  }
}

Mode DisplayUI::getMode() { return currentMode; }
