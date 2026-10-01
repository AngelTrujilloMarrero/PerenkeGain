#pragma once
#include <JuceHeader.h>
#include "dsp/KWeightingFilter.h"

namespace pg {

// Analisis de sonoridad offline (aprox. ReplayGain): aplica ponderacion K al
// audio, lo mezcla a mono y devuelve el RMS en dBFS. Sirve para normalizar
// por volumen percibido, no por pico.
class LoudnessAnalyzer {
public:
  // RMS K-weighted en dBFS (silencio -> -100).
  static float rmsDbF(const juce::AudioBuffer<float> &buf, double sampleRate);
};

} // namespace pg
