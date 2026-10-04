#include "ui/MeterClock.h"
#include "audio/AudioEngine.h"
#include "dsp/BandLevelAnalyzer.h"
#include "ui/MixerBarComponent.h"
#include "ui/BottomDockComponent.h"
#include "app/PlaybackTicker.h"

namespace pg {

MeterClock::MeterClock(AudioEngine &e, MixerBarComponent &m,
                       BottomDockComponent &d, PlaybackTicker &t)
    : engine(e), mixer(m), dock(d), ticker(t) {
  startTimerHz(5);
}

bool MeterClock::metersAtFloor() const {
  const auto &bands = engine.bandAnalyzer();
  for (int b = 0; b < BandLevelAnalyzer::kBands; ++b)
    if (bands.levelDb(b) > -59.9f)
      return false;
  return bands.masterLevelDb(0) <= -59.9f &&
         bands.masterLevelDb(1) <= -59.9f;
}

void MeterClock::timerCallback() {
  if (!engine.anyDeckPlaying() && metersAtFloor())
    return;
  mixer.tickMeters();
  dock.tickMeters();
  ticker.tick();
}

} // namespace pg
