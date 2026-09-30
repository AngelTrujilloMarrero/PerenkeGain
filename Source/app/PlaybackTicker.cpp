#include "app/PlaybackTicker.h"

namespace pg {

PlaybackTicker::PlaybackTicker(AudioEngine &e, FileInfoBar &f,
                               CyanProgressBar &p, WaveformComponent &w)
    : engine(e), info(f), bar(p), wave(w) {
  startTimerHz(30);
}

void PlaybackTicker::timerCallback() {
  double pos = engine.getPositionSec();
  double len = engine.getLengthSec();
  info.setPosition(pos);
  bar.setFraction(len > 0.0 ? pos / len : 0.0);
  wave.setPlayhead(engine.hasFile() ? pos : -1.0);
}

} // namespace pg
