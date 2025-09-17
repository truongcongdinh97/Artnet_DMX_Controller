#pragma once
#include <Arduino.h>

enum Mode { STREAMING, RECORDING, PLAYBACK };

namespace DisplayUI {
  void begin();
  void loop();
  Mode getMode();
}
