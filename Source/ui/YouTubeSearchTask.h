#pragma once
#include <JuceHeader.h>
#include <functional>
#include <vector>
#include "storage/YouTubeSearch.h"

namespace pg {

// Hilo de busqueda en YouTube Music, sin ventana. Avisa del resultado por
// callback en el hilo de mensajes.
class YouTubeSearchTask : public juce::Thread {
public:
  using DoneFn =
      std::function<void(std::vector<YouTubeSearchItem>, juce::String)>;

  YouTubeSearchTask(juce::String query, int maxResults, DoneFn onDone);
  void run() override;

private:
  juce::String query;
  int maxResults;
  DoneFn onDone;
};

} // namespace pg
