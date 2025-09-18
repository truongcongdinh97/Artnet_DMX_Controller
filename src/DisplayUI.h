#pragma once
#include <Arduino.h>

enum Mode { STREAMING, RECORDING, PLAYBACK, CONFIG };

namespace DisplayUI {
  void begin();
  void loop();
  Mode getMode();
  void showRecordStatus(bool isRecording, uint32_t recordMillis); // Hiển thị trạng thái record và thời gian
  void showPlaybackStatus(const String& filename, uint32_t playMillis); // Hiển thị playback file và thời gian
}
