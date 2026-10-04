#pragma once
#include <JuceHeader.h>

namespace pg {

// Sonoridad integrada offline (ITU-R BS.1770 / EBU R128 simplificado).
// Una sola cosa: buffer K-weighted -> LUFS integrado con gate.
// Bloques de 400 ms (hop 100 ms), gate absoluto -70 LUFS y relativo -10 LU.
class IntegratedLoudness {
public:
  // LUFS integrado (silencio -> -100). Suma energias por canal (G=1.0).
  static float analyze(const juce::AudioBuffer<float> &buf, double sampleRate);

  // Conversion a escala mp3gain (89 dB == -14 LUFS).
  static constexpr float toMp3GainDb(float lufs) { return lufs + 103.0f; }
  static constexpr float fromMp3GainDb(float db) { return db - 103.0f; }
};

} // namespace pg
