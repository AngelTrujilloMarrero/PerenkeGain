#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "audio/WaveformComponent.h"
#include "ui/FileInfoBar.h"
#include "ui/CyanProgressBar.h"

namespace pg {

// Actualiza info, barra cian y playhead ~30 veces por segundo.
class PlaybackTicker : public juce::Timer {
public:
  PlaybackTicker(AudioEngine &e, FileInfoBar &f, CyanProgressBar &p,
                 WaveformComponent &w);
  void timerCallback() override;

private:
  AudioEngine &engine;
  FileInfoBar &info;
  CyanProgressBar &bar;
  WaveformComponent &wave;
};

} // namespace pg
