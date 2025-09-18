#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <Arduino.h>
#include <vector>
#include "PlaybackController.h"

enum OperatingMode {
    MODE_STREAMING,
    MODE_RECORDING,
    MODE_PLAYBACK,
    MODE_CONFIG
};

namespace MenuManager {
    void begin();
    void loop();
    OperatingMode getMode();
    String getSelectedFile();
    PlaybackMode getLoopMode();
    void setFileList(const std::vector<String>& files);
    // --- Bổ sung cho record ---
    void startRecord();
    void stopRecord();
    bool isRecording();
    uint32_t getRecordMillis();
    // --- Bổ sung cho playback ---
    uint32_t getPlaybackMillis();
    void resetPlaybackMillis();
}

#endif
