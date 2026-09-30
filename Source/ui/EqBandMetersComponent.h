#pragma once
#include <JuceHeader.h>
#include <array>
#include "dsp/BandLevelAnalyzer.h"

namespace pg {

// Fila de 31 medidores verticales (uno por banda del ecualizador),
// estilo clásico: pozo hundido, relleno verde y rojo al saturar.
class EqBandMetersComponent : public juce::Component,
                              private juce::Timer {
public:
  EqBandMetersComponent();
  void paint(juce::Graphics &g) override;
  void attach(const BandLevelAnalyzer *a) { analyzer = a; }

private:
  void timerCallback() override;
  const BandLevelAnalyzer *analyzer = nullptr;
  std::array<float, BandLevelAnalyzer::kBands> shown{};
};

} // namespace pg
