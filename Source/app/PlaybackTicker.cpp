#include "app/PlaybackTicker.h"
#include <cmath>

namespace pg {

PlaybackTicker::PlaybackTicker(AudioEngine &e, CyanProgressBar &p,
                               juce::Label &pct)
    : engine(e), bar(p), percent(pct) {
  startTimerHz(30);
}

void PlaybackTicker::timerCallback() {
  const int d = engine.activeDeck();
  const double pos = engine.getPositionSec(d);
  const double len = engine.getLengthSec(d);
  const double frac = len > 0.0 ? juce::jlimit(0.0, 1.0, pos / len) : 0.0;
  bar.setFraction(frac);
  percent.setText(juce::String((int)std::lround(frac * 100.0)) + "%",
                  juce::dontSendNotification);
}

} // namespace pg
