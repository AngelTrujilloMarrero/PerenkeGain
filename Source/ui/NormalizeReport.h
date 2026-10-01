#pragma once
#include <JuceHeader.h>
#include <vector>
#include "storage/BatchNormalizer.h"

namespace pg {

// Plan de ganancias calculado a partir del analisis y del objetivo.
struct NormalizePlan {
  std::vector<float> trackGains;
  float albumGainDb = 0.0f;
};

// Calcula la ganancia por pista y la de album (energia combinada). Si
// preventClip, limita para no pasar de 0 dBFS.
NormalizePlan planNormalization(const std::vector<NormalizeAnalysis> &analyses,
                                float targetDb, bool preventClip);

// Texto con la tabla de resultados (fichero, volumen, pico, ganancia).
juce::String formatNormalizeReport(const std::vector<NormalizeAnalysis> &analyses,
                                   const NormalizePlan &plan,
                                   const juce::String &title = {});

} // namespace pg
