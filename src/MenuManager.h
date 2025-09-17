#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <Arduino.h>
#include <vector>
#include "PlaybackController.h"

enum OperatingMode {
    MODE_STREAMING,
    MODE_RECORDING,
    MODE_PLAYBACK
};

namespace MenuManager {
    void begin();
    void loop();
    OperatingMode getMode();
    String getSelectedFile();
    PlaybackMode getLoopMode();
    void setFileList(const std::vector<String>& files);
}

#endif
