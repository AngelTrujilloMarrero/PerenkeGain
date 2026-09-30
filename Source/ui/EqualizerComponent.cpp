#include "ui/EqualizerComponent.h"

namespace pg {

EqualizerComponent::EqualizerComponent() {
  for (size_t i = 0; i < 31; ++i) {
    gains[i].setRange(-12.0, 12.0, 0.1);
    gains[i].setValue(0.0);
    gains[i].setSliderStyle(juce::Slider::LinearVertical);
    addAndMakeVisible(gains[i]);
    intensities[i].setRange(0.0, 100.0, 1.0);
    intensities[i].setValue(100.0);
    intensities[i].setSliderStyle(juce::Slider::LinearVertical);
    addAndMakeVisible(intensities[i]);
  }
  master.setRange(0.0, 100.0, 1.0);
  master.setValue(100.0);
  addAndMakeVisible(master);
}

void EqualizerComponent::resized() {
  auto r = getLocalBounds();
  auto top = r.removeFromTop(int(r.getHeight() * 0.7));
  auto bot = r.removeFromTop(int(r.getHeight() * 0.6));
  int w = top.getWidth() / 31;
  for (size_t i = 0; i < 31; ++i) {
    gains[i].setBounds(top.removeFromLeft(w).reduced(1));
    intensities[i].setBounds(bot.removeFromLeft(w).reduced(1));
  }
  master.setBounds(r.reduced(4));
}

Eq31State EqualizerComponent::getState() const {
  Eq31State s{};
  for (size_t i = 0; i < 31; ++i) {
    s.bands[i].gainDb = (float)gains[i].getValue();
    s.bands[i].intensity = (float)intensities[i].getValue() / 100.f;
  }
  s.masterIntensity = (float)master.getValue() / 100.f;
  return s;
}

} // namespace pg
