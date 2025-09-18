#include "ShowAudioLink.h"
#include <SD.h>
#include <Preferences.h>

namespace ShowAudioLink {
  static Preferences prefs;

  void begin() {
    prefs.begin("showaudiolink", false);
  }

  std::vector<String> listShows(const char* path) {
    std::vector<String> shows;
    File dir = SD.open(path);
    if (!dir) return shows;
    File file;
    while ((file = dir.openNextFile())) {
      String name = file.name();
      if (!file.isDirectory() && name.endsWith(".dat")) {
        shows.push_back(name);
      }
      file.close();
    }
    dir.close();
    return shows;
  }

  std::vector<String> listAudio(const char* path) {
    std::vector<String> audioFiles;
    File dir = SD.open(path);
    if (!dir) return audioFiles;
    File file;
    while ((file = dir.openNextFile())) {
      String name = file.name();
      if (!file.isDirectory() && (name.endsWith(".mp3") || name.endsWith(".MP3") || name.endsWith(".wav") || name.endsWith(".WAV"))) {
        audioFiles.push_back(name);
      }
      file.close();
    }
    dir.close();
    return audioFiles;
  }

  void setLink(const String& show, const String& audio) {
    prefs.putString(show.c_str(), audio);
  }

  String getLink(const String& show) {
    return prefs.getString(show.c_str(), "");
  }
}