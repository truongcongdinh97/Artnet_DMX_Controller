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

// Mapping động: mỗi output có thể dùng nhiều universe, không cố định +4
// Số universe/output = ceil(ledsPerOutput*3/512)
#include "ConfigManager.h"
#include <math.h>
void LEDController::updateFromArtnet(uint16_t universe, uint16_t length, uint8_t* data) {
  Config cfg = ConfigManager::getConfig();
  int outputs = cfg.outputs;
  int ledsPerOutput = cfg.ledsPerOutput;
  int startUniverse = cfg.startUniverse;
  int channelsPerLed = 3;
  int dmxPerUni = 512;

  // Tính offset universe cho từng output
  int outputUniverseStart[MAX_OUTPUTS];
  int outputUniverseCount[MAX_OUTPUTS];
  int nextUni = startUniverse;
  for (int o = 0; o < outputs; o++) {
    int nLed = ledsPerOutput;
    int nUni = (int)ceil((nLed * channelsPerLed) / (float)dmxPerUni);
    outputUniverseStart[o] = nextUni;
    outputUniverseCount[o] = nUni;
    nextUni += nUni;
  }

  // Tìm output nào nhận universe này
  for (int o = 0; o < outputs; o++) {
    int uStart = outputUniverseStart[o];
    int uEnd = uStart + outputUniverseCount[o] - 1;
    if (universe >= uStart && universe <= uEnd) {
      // Tính offset LED trong output
      int uniOffset = universe - uStart;
      int ledOffset = (uniOffset * dmxPerUni) / channelsPerLed;
      int maxLed = ledsPerOutput;
      int nLed = length / channelsPerLed;
      if (ledOffset + nLed > maxLed) nLed = maxLed - ledOffset;
      for (int i = 0; i < nLed; i++) {
        int di = i * channelsPerLed;
        leds[o][ledOffset + i] = CRGB(data[di], data[di+1], data[di+2]);
      }
      break;
    }
  }
}
