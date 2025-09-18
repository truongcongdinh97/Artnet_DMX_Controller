#pragma once
#include <Arduino.h>
#include <vector>

namespace ShowAudioLink {
  // Initialize preferences and any internal state
  void begin();

  // List recorded show files on SD (e.g., files with .dmx or .rec extension)
  std::vector<String> listShows(const char* path = "/");

  // List audio files on SD (e.g., .mp3, .wav)
  std::vector<String> listAudio(const char* path = "/");

  // Link a show file to an audio file
  void setLink(const String& show, const String& audio);

  // Get the linked audio file for a given show, empty if none
  String getLink(const String& show);
}