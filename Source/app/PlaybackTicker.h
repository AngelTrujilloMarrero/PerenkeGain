#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "ui/CyanProgressBar.h"

namespace pg {

// Actualiza la barra cian de progreso (deck activo).
// Lo llama MeterClock (tick unico compartido).
class PlaybackTicker {
public:
  PlaybackTicker(AudioEngine &e, CyanProgressBar &p, juce::Label &pct);
  void tick();

private:
  AudioEngine &engine;
  CyanProgressBar &bar;
  juce::Label &percent;
};

} // namespace pg
