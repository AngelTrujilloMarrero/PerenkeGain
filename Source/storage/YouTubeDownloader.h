#pragma once
#include <JuceHeader.h>
#include <functional>

namespace pg {

struct YouTubeResult {
  bool ok = false;
  juce::File file;
  juce::String error;
};

// Descarga audio de YouTube/YouTube Music con yt-dlp (extraccion a MP3 con
// ffmpeg). Requiere que ambas herramientas esten en el PATH.
class YouTubeDownloader {
public:
  static bool available(juce::File &ytdlp, juce::File &ffmpeg);
  static YouTubeResult download(const juce::String &url, const juce::File &outDir,
                                const std::function<void(float)> &onProgress);
};

} // namespace pg
