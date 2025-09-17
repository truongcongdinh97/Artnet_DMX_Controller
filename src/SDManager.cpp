#include "SDManager.h"
#include <SD.h>


static File activeFile;
static bool playing = false;

void SDManager::begin() {
  if (!SD.begin()) {
    Serial.println("SD init failed!");
    return;
  }
  Serial.println("SD initialized.");
}

void SDManager::loop() {
  if (playing) {
    // TODO: read data and push to LEDController
  }
}



void SDManager::startPlayback(const char* filename) {
  activeFile = SD.open(filename);
  if (activeFile) playing = true;
}

void SDManager::stopPlayback() {
  if (activeFile) activeFile.close();
  playing = false;
}

std::vector<String> SDManager::listFiles(const char* path) {
  std::vector<String> files;
  File dir = SD.open(path);
  if (!dir || !dir.isDirectory()) {
    return files;
  }
  File entry;
  while ((entry = dir.openNextFile())) {
    if (!entry.isDirectory()) {
      files.push_back(String(entry.name()));
    }
    entry.close();
  }
  dir.close();
  return files;
}
