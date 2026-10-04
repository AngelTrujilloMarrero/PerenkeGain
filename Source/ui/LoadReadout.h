#pragma once
#include <JuceHeader.h>

namespace pg {

class AudioEngine;

// Etiqueta compacta que muestra el uso de CPU del proceso y la carga del
// hilo de audio (callback). Se actualiza a 2 Hz.
class LoadReadout : public juce::Label, private juce::Timer {
public:
  explicit LoadReadout(AudioEngine &e);

private:
  void timerCallback() override;

  AudioEngine &engine;
  double lastCpuSeconds = -1.0;
  double lastWallSeconds = -1.0;
};

} // namespace pg
