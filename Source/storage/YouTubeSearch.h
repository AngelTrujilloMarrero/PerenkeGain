#pragma once
#include <JuceHeader.h>
#include <vector>

namespace pg {

struct YouTubeSearchItem {
  juce::String title;
  juce::String url;
  juce::String duration;
};

// Busca en YouTube/YouTube Music con yt-dlp (ytsearch). Devuelve titulos y
// URLs; requiere yt-dlp en el PATH.
class YouTubeSearch {
public:
  static std::vector<YouTubeSearchItem> search(const juce::String &query,
                                               int maxResults,
                                               juce::String &error);
};

} // namespace pg
