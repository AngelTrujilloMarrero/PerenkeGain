#pragma once
#include <JuceHeader.h>
#include "storage/YouTubeDownloader.h"
#include "storage/YouTubeSearch.h"

namespace pg {

// Puente JNI al YouTubeBridge de Java (Chaquopy + yt-dlp + ffmpeg-kit).
// Solo compila en Android; en escritorio las clases usan yt-dlp/ffmpeg
// del sistema via ExternalTool.
#if JUCE_ANDROID
class YouTubeAndroid {
public:
  static std::vector<YouTubeSearchItem>
  search(const juce::String &query, int maxResults,
         juce::String &error);
  static YouTubeResult
  download(const juce::String &url, const juce::File &outDir);
};
#endif

} // namespace pg
