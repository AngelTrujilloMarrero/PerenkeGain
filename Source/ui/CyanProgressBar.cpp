#include "ui/CyanProgressBar.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

void CyanProgressBar::setFraction(double f) {
  fraction = juce::jlimit(0.0, 1.0, f);
  repaint();
}

void CyanProgressBar::paint(juce::Graphics &g) {
  auto r = getLocalBounds();
  g.setColour(juce::Colours::white);
  g.fillRect(r);
  g.setColour(juce::Colour(0xFF00E0E0));
  g.fillRect(r.removeFromLeft(int(r.getWidth() * fraction)));
  g.setColour(juce::Colour(0xFF808080));
  g.drawRect(getLocalBounds(), 1);
  retro::bevelSunken(g, getLocalBounds().reduced(1));
}

} // namespace pg
