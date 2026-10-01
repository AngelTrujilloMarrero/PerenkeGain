#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "ui/CyanProgressBar.h"

namespace pg {

// Actualiza la barra cian de progreso ~30 veces por segundo (deck activo).
class PlaybackTicker : public juce::Timer {
public:
  PlaybackTicker(AudioEngine &e, CyanProgressBar &p, juce::Label &pct);
  void timerCallback() override;

private:
  AudioEngine &engine;
  CyanProgressBar &bar;
  juce::Label &percent;
};

} // namespace pg
