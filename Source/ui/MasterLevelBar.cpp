#include "ui/MasterLevelBar.h"

namespace pg {

MasterLevelBar::MasterLevelBar() {
  setOpaque(true);
  startTimerHz(8);
}

void MasterLevelBar::timerCallback() {
  if (!isShowing() || analyzer == nullptr)
    return;
  bool changed = false;
  for (int ch = 0; ch < 2; ++ch) {
    const float next = analyzer->masterLevelDb(ch);
    if (std::abs(next - levelDb[ch]) > 0.1f) {
      levelDb[ch] = next;
      changed = true;
    }
  }
  if (changed)
    repaint();
}

void MasterLevelBar::paint(juce::Graphics &g) {
  auto r = getLocalBounds();
  g.setColour(juce::Colour(0xFF0B0C10));
  g.fillAll();

  const int gap = 2;
  const int w = juce::jmax(1, (r.getWidth() - gap) / 2);
  for (int ch = 0; ch < 2; ++ch) {
    auto col = juce::Rectangle<int>(r.getX() + ch * (w + gap), r.getY(), w,
                                    r.getHeight());
    g.setColour(juce::Colour(0xFF14151A));
    g.fillRect(col);
    const float frac =
        juce::jlimit(0.0f, 1.0f, (levelDb[ch] + 60.0f) / 60.0f);
    if (frac > 0.0f) {
      auto fill = col.reduced(1);
      const int h = int(std::round(fill.getHeight() * frac));
      fill = fill.removeFromBottom(juce::jmax(1, h));
      juce::ColourGradient grad(juce::Colour(0xFFFF4136), 0.0f,
                                (float)col.getY(),
                                juce::Colour(0xFF2ECC40), 0.0f,
                                (float)col.getBottom(), false);
      grad.addColour(0.3, juce::Colour(0xFFFFDC00));
      g.setGradientFill(grad);
      g.fillRect(fill);
    }
    g.setColour(juce::Colours::black);
    g.drawRect(col, 1);
  }
}

} // namespace pg
