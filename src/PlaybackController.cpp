#include "PlaybackController.h"
#include "SDManager.h"

std::vector<String> PlaybackController::files;
int PlaybackController::currentIndex = -1;
PlaybackMode PlaybackController::playbackMode = PLAY_ONCE;
bool PlaybackController::playing = false;
unsigned long PlaybackController::lastFrameTime = 0;

void PlaybackController::begin() {
    files.clear();
    currentIndex = -1;
    playing = false;
}

void PlaybackController::loop() {
    if (!playing) return;
    handlePlayback();
}

void PlaybackController::setFiles(const std::vector<String>& fileList) {
    files = fileList;
    currentIndex = files.empty() ? -1 : 0;
}

void PlaybackController::setMode(PlaybackMode mode) {
    playbackMode = mode;
}

void PlaybackController::playFile(const String& filename) {
    // tìm file trong danh sách
    for (size_t i = 0; i < files.size(); i++) {
        if (files[i] == filename) {
            currentIndex = i;
            loadFile(filename);
            playing = true;
            return;
        }
    }
}

void PlaybackController::stop() {
    playing = false;
}

void PlaybackController::next() {
    if (files.empty()) return;
    currentIndex = (currentIndex + 1) % files.size();
    loadFile(files[currentIndex]);
}

void PlaybackController::previous() {
    if (files.empty()) return;
    currentIndex = (currentIndex - 1 + files.size()) % files.size();
    loadFile(files[currentIndex]);
}

bool PlaybackController::isPlaying() {
    return playing;
}

String PlaybackController::currentFile() {
    if (currentIndex >= 0 && currentIndex < (int)files.size()) {
        return files[currentIndex];
    }
    return "";
}

void PlaybackController::loadFile(const String& filename) {
    // TODO: mở file từ SD, chuẩn bị đọc frame
    Serial.printf("▶️ Loading file: %s\n", filename.c_str());
    lastFrameTime = millis();
}

void PlaybackController::handlePlayback() {
    unsigned long now = millis();
    if (now - lastFrameTime >= 33) { // giả sử 30 FPS
        lastFrameTime = now;

        // TODO: đọc frame từ SDManager và gửi ra LED
        // LEDController::showFrame(frameData);

        // TODO: nếu hết file
        bool fileEnded = false;
        if (fileEnded) {
            if (playbackMode == PLAY_ONCE) {
                stop();
            } else if (playbackMode == LOOP_ONE) {
                loadFile(files[currentIndex]);
            } else if (playbackMode == LOOP_ALL) {
                next();
            }
        }
    }
}
