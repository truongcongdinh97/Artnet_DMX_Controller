
#include "StatusLED.h"
#include "PinConfig.h"
#include <FastLED.h>

namespace StatusLED {
  static CRGB led[1];
  static uint8_t mode = 0; // 0:stream, 1:record, 2:play
  static uint32_t lastBlink = 0;
  static bool ledOn = true;
  // static uint8_t pinUsed = GPIO_STATUS_LED;

  void begin(uint8_t pin) {
    // pin argument ignored, always use GPIO_STATUS_LED for safety
    FastLED.addLeds<WS2812, GPIO_STATUS_LED, GRB>(led, 1);
    setStreaming();
    FastLED.show();
  }

  void setStreaming() {
    mode = 0;
    led[0] = CRGB::Blue;
    FastLED.show();
  }
  void setRecording() {
    mode = 1;
    led[0] = CRGB::Red;
    FastLED.show();
    lastBlink = millis();
    ledOn = true;
  }
  void setPlayback() {
    mode = 2;
    led[0] = CRGB::Green;
    FastLED.show();
  }
  void loop() {
    if (mode == 1) { // Recording: nhấp nháy đỏ
      if (millis() - lastBlink > 400) {
        ledOn = !ledOn;
        led[0] = ledOn ? CRGB::Red : CRGB::Black;
        FastLED.show();
        lastBlink = millis();
      }
    }
  }
}
