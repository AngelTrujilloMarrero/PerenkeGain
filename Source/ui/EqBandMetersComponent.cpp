#include "ui/EqBandMetersComponent.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

EqBandMetersComponent::EqBandMetersComponent() {
  shown.fill(-60.0f);
  startTimerHz(30);
}

void EqBandMetersComponent::timerCallback() {
  if (!isShowing() || analyzer == nullptr)
    return;
  for (int b = 0; b < BandLevelAnalyzer::kBands; ++b)
    shown[(size_t)b] = analyzer->levelDb(b);
  repaint();
}

void EqBandMetersComponent::paint(juce::Graphics &g) {
  auto r = getLocalBounds();
  constexpr int n = BandLevelAnalyzer::kBands;
  int w = r.getWidth() / n;
  for (int b = 0; b < n; ++b) {
    auto col = r.removeFromLeft(w);
    if (b == n - 1)
      col.setWidth(r.getWidth()); // la última banda usa el resto
    col = col.reduced(1, 0);
    retro::bevelSunken(g, col);
    auto inner = col.reduced(2);
    g.setColour(juce::Colours::white);
    g.fillRect(inner);
    float t = juce::jlimit(0.0f, 1.0f, (shown[(size_t)b] + 60.0f) / 60.0f);
    int fh = juce::roundToInt(t * (float)inner.getHeight());
    if (fh > 0) {
      g.setColour(t > 0.85f ? juce::Colour(0xFFCC2020)
                            : juce::Colour(0xFF208020));
      g.fillRect(inner.removeFromBottom(fh));
    }
  }
}

} // namespace pg
