#pragma once
#include <JuceHeader.h>
#include "dsp/BandLevelAnalyzer.h"

namespace pg {

// Medidor master de 2 barras verticales (L/R), alimentado por el analizador
// de salida. Va colocado junto a la forma de onda. Rango -60..0 dB con rampa
// verde/ambar/rojo.
class MasterLevelBar : public juce::Component, private juce::Timer {
public:
  MasterLevelBar();
  void attach(const BandLevelAnalyzer *a) { analyzer = a; }
  void paint(juce::Graphics &g) override;

private:
  void timerCallback() override;
  const BandLevelAnalyzer *analyzer = nullptr;
  float levelDb[2] = {-60.0f, -60.0f};
};

} // namespace pg
