#pragma once
#include <JuceHeader.h>

namespace pg {

// Motor playback pasivo (solo editor, sin captura).
class AudioEngine {
public:
  AudioEngine();
  void loadFile(const juce::File &f);
  void play();
  void stop();
  double getLengthSec() const;

private:
  juce::AudioFormatManager formats;
  juce::AudioTransportSource transport;
  std::unique_ptr<juce::AudioFormatReaderSource> reader;
};

} // namespace pg
