#include "app/PlaybackTicker.h"
#include <cmath>

namespace pg {

PlaybackTicker::PlaybackTicker(AudioEngine &e, CyanProgressBar &p,
                               juce::Label &pct)
    : engine(e), bar(p), percent(pct) {
  startTimerHz(20);
}

void PlaybackTicker::timerCallback() {
  const int d = engine.activeDeck();
  const double pos = engine.getPositionSec(d);
  const double len = engine.getLengthSec(d);
  const double frac = len > 0.0 ? juce::jlimit(0.0, 1.0, pos / len) : 0.0;
  bar.setFraction(frac);
  const juce::String txt =
      juce::String((int)std::lround(frac * 100.0)) + "%";
  if (percent.getText() != txt)
    percent.setText(txt, juce::dontSendNotification);
}

} // namespace pg
