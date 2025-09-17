#include "LEDController.h"
#include <FastLED.h>
#include "PinConfig.h"

// Tối đa 4 outputs, mỗi output 680 LED
#define MAX_OUTPUTS 4
#define MAX_LEDS_PER_OUTPUT 680

static CRGB leds[MAX_OUTPUTS][MAX_LEDS_PER_OUTPUT];

void LEDController::begin() {
  FastLED.addLeds<WS2812B, GPIO_LED_OUT_1, GRB>(leds[0], MAX_LEDS_PER_OUTPUT);
  FastLED.addLeds<WS2812B, GPIO_LED_OUT_2, GRB>(leds[1], MAX_LEDS_PER_OUTPUT);
  FastLED.addLeds<WS2812B, GPIO_LED_OUT_3, GRB>(leds[2], MAX_LEDS_PER_OUTPUT);
  FastLED.addLeds<WS2812B, GPIO_LED_OUT_4, GRB>(leds[3], MAX_LEDS_PER_OUTPUT);
  FastLED.clear();
  FastLED.show();
  Serial.println("LEDController started.");
}

void LEDController::loop() {
  FastLED.show();
}

void LEDController::updateFromArtnet(uint16_t universe, uint16_t length, uint8_t* data) {
  for (int i = 0; i < length / 3; i++) {
    leds[0][i] = CRGB(data[i*3], data[i*3+1], data[i*3+2]);
  }
}
