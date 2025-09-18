#include "StatusLED.h"
#include "PinConfig.h"
#include <Arduino.h>

namespace StatusLED {
  static uint8_t mode = 0; // 0:stream, 1:record, 2:play
  static uint32_t lastBlink = 0;
  static bool ledOn = true;

  void begin(uint8_t pin) {
    // pin argument ignored, always use GPIO_STATUS_LED for safety
    pinMode(GPIO_STATUS_LED, OUTPUT);
    setStreaming();
  }

  void setStreaming() {
    mode = 0;
    digitalWrite(GPIO_STATUS_LED, HIGH);
  }
  void setRecording() {
    mode = 1;
    ledOn = true;
    digitalWrite(GPIO_STATUS_LED, HIGH);
    lastBlink = millis();
  }
  void setPlayback() {
    mode = 2;
    digitalWrite(GPIO_STATUS_LED, HIGH);
  }
  void loop() {
    if (mode == 1) { // Recording: nhấp nháy đỏ
      if (millis() - lastBlink > 400) {
        ledOn = !ledOn;
        digitalWrite(GPIO_STATUS_LED, ledOn ? HIGH : LOW);
        lastBlink = millis();
      }
    }
  }
}
