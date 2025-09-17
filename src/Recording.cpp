#include "Recording.h"
#include <SD.h>

namespace Recording {
  static File activeFile;
  static bool recording = false;

  void begin() {
    // Optionally initialize SD or other resources here
  }

  void loop() {
    // No periodic action needed for recording; handled by callback
  }

  void start() {
    // Tìm số file ShowNNN.dat hiện có
    int idx = 0;
    char fname[20];
    do {
      snprintf(fname, sizeof(fname), "/Show%03d.dat", idx);
      if (!SD.exists(fname)) break;
      idx++;
    } while (idx < 1000);
    activeFile = SD.open(fname, FILE_WRITE);
    if (activeFile) recording = true;
    Serial.print("[Recording] Start file: ");
    Serial.println(fname);
  }

  void stop() {
    if (activeFile) activeFile.close();
    recording = false;
  }

  void recordPacket(uint16_t universe, uint16_t length, const uint8_t* data) {
    if (!recording || !activeFile) return;
    activeFile.write((uint8_t*)&universe, 2);
    activeFile.write((uint8_t*)&length, 2);
    activeFile.write(data, length);
    activeFile.flush();
  }

  bool isRecording() {
    return recording;
  }
}
