#include "ui/MeterClock.h"
#include "ui/MixerBarComponent.h"
#include "ui/BottomDockComponent.h"
#include "app/PlaybackTicker.h"

namespace pg {

MeterClock::MeterClock(MixerBarComponent &m, BottomDockComponent &d,
                       PlaybackTicker &t)
    : mixer(m), dock(d), ticker(t) {
  startTimerHz(8);
}

void MeterClock::timerCallback() {
  mixer.tickMeters();
  dock.tickMeters();
  ticker.tick();
}

} // namespace pg
