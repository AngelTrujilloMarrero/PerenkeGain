#pragma once
#include <JuceHeader.h>

namespace pg {

// Lienzo waveform estéreo L/R con AudioThumbnail.
class WaveformComponent : public juce::Component,
                          public juce::ChangeListener {
public:
  WaveformComponent();
  void paint(juce::Graphics &g) override;
  void changeListenerCallback(juce::ChangeBroadcaster *) override;
  void openFile(const juce::File &f);

private:
  juce::AudioFormatManager formats;
  juce::AudioThumbnailCache cache{5};
  juce::AudioThumbnail thumb{512, formats, cache};
};

} // namespace pg
