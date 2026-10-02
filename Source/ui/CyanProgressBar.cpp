#include "ui/CyanProgressBar.h"
#include "ui/ModernLookAndFeel.h"

namespace pg {

void CyanProgressBar::setFraction(double f) {
  fraction = juce::jlimit(0.0, 1.0, f);
  repaint();
}

void CyanProgressBar::mouseDown(const juce::MouseEvent &e) { seekTo(e); }

void CyanProgressBar::mouseDrag(const juce::MouseEvent &e) { seekTo(e); }

void CyanProgressBar::seekTo(const juce::MouseEvent &e) {
  if (getWidth() <= 0 || !onSeekFraction)
    return;
  onSeekFraction(juce::jlimit(0.0, 1.0,
                              (double)e.position.x / (double)getWidth()));
}

void CyanProgressBar::paint(juce::Graphics &g) {
  auto r = getLocalBounds().toFloat();
  g.setColour(juce::Colour(0xFF2A2E38));
  g.fillRoundedRectangle(r, 4.0f);
  g.setColour(modern::accent());
  g.fillRoundedRectangle(
      juce::Rectangle<float>(r.getX(), r.getY(),
                             r.getWidth() * (float)fraction, r.getHeight()),
      4.0f);
  modern::frame(g, getLocalBounds());
}

} // namespace pg
