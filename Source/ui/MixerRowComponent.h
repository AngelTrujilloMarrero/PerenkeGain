#pragma once
#include <JuceHeader.h>
#include "dsp/BandLevelAnalyzer.h"
#include "ui/MixerStripComponent.h"

namespace pg {

// Fila de 31 tiras de canal estilo mesa de sonido digital moderna:
// cada tira = medidor LED + fader de ganancia (sube/baja) + botón MUTE.
class MixerRowComponent : public juce::Component,
                          private juce::Timer {
public:
  MixerRowComponent();
  void resized() override;
  void attach(const BandLevelAnalyzer *a) { analyzer = a; }

  float gainDb(int i) const;
  void setGainDb(int i, float db);
  bool isMuted(int i) const;
  void setMuted(int i, bool muted);

  // Actualiza el lector LCD del EQ al editar una tira.
  std::function<void(const juce::String &)> onReadout;

private:
  void timerCallback() override;
  juce::String infoFor(int i) const;

  std::array<std::unique_ptr<MixerStripComponent>, 31> strips;
  const BandLevelAnalyzer *analyzer = nullptr;
};

} // namespace pg
