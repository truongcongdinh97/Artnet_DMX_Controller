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
    activeFile = SD.open("/record.dat", FILE_WRITE);
    if (activeFile) recording = true;
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
