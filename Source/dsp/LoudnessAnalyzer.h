#pragma once
#include <JuceHeader.h>
#include "dsp/KWeightingFilter.h"

namespace pg {

// Analisis de sonoridad offline (BS.1770 integrado con gate): delega en
// IntegratedLoudness. Suma energias por canal y descarta silencios, por eso
// se aproxima a mp3gain/ReplayGain mejor que un RMS global.
// Devuelve LUFS integrado en dBFS (silencio -> -100).
class LoudnessAnalyzer {
public:
  // LUFS integrado (silencio -> -100). Nombre historico rmsDbF.
  static float rmsDbF(const juce::AudioBuffer<float> &buf, double sampleRate);
};

} // namespace pg
