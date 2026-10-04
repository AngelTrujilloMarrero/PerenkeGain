#pragma once
#include <JuceHeader.h>

namespace pg {

class AudioEngine;
class MixerBarComponent;
class BottomDockComponent;
class PlaybackTicker;

// Reloj unico para todos los medidores de la UI (~8 Hz). Antes cada
// medidor tenia su propio Timer desfasado y cada tick provocaba un render
// de ventana completa, saturando la CPU en reproduccion. Con un solo tick
// todos los repintados caen en el mismo frame. En pausa y con medidores
// ya en suelo no hace nada.
class MeterClock : private juce::Timer {
public:
  MeterClock(AudioEngine &engine, MixerBarComponent &mixer,
             BottomDockComponent &dock, PlaybackTicker &ticker);

private:
  void timerCallback() override;
  bool metersAtFloor() const;

  AudioEngine &engine;
  MixerBarComponent &mixer;
  BottomDockComponent &dock;
  PlaybackTicker &ticker;
};

} // namespace pg
