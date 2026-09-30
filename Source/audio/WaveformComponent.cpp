#include "audio/WaveformComponent.h"

namespace pg {

WaveformComponent::WaveformComponent() {
  formats.registerBasicFormats();
  thumb.addChangeListener(this);
}

void WaveformComponent::openFile(const juce::File &f) {
  thumb.setSource(new juce::FileInputSource(f));
}

void WaveformComponent::changeListenerCallback(juce::ChangeBroadcaster *) {
  repaint();
}

void WaveformComponent::paint(juce::Graphics &g) {
  g.fillAll(juce::Colours::black.brighter(0.08f));
  if (thumb.getNumChannels() == 0) {
    g.setColour(juce::Colours::grey);
    g.drawFittedText("Abre un WAV/MP3/FLAC", getLocalBounds(),
                     juce::Justification::centred, 1);
    return;
  }
  g.setColour(juce::Colours::lightgreen);
  thumb.drawChannels(g, getLocalBounds().reduced(8),
                     0.0, thumb.getTotalLength(), 1.0f);
}

} // namespace pg
