#pragma once
#include <Arduino.h>
#include <vector>
#include <string>

enum PlaybackMode {
    PLAY_ONCE,
    LOOP_ONE,
    LOOP_ALL
};

class PlaybackController {
public:
    static void begin();
    static void loop();

    static void setFiles(const std::vector<String>& fileList);
    static void setMode(PlaybackMode mode);
    static void playFile(const String& filename);
    static void stop();
    static void next();
    static void previous();

    static bool isPlaying();
    static String currentFile();

private:
    static std::vector<String> files;
    static int currentIndex;
    static PlaybackMode playbackMode;
    static bool playing;
    static unsigned long lastFrameTime;

    static void loadFile(const String& filename);
    static void handlePlayback();
};
