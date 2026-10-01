#pragma once
#include <JuceHeader.h>
#include <functional>
#include "storage/YouTubeDownloader.h"

namespace pg {

// Hilo de descarga + normalizacion, sin ventana. Avisa del resultado por
// callback en el hilo de mensajes.
class YouTubeTask : public juce::Thread {
public:
  using DoneFn =
      std::function<void(juce::Array<juce::File>, juce::StringArray)>;

  YouTubeTask(std::vector<juce::String> urls, juce::File outDir, float targetDb,
              DoneFn onDone);
  void run() override;

private:
  void normalizeInPlace(YouTubeResult &res);

  std::vector<juce::String> urls;
  juce::File outDir;
  float target;
  DoneFn onDone;
  juce::Array<juce::File> okFiles;
  juce::StringArray errors;
};

} // namespace pg
