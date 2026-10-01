#pragma once
#include <JuceHeader.h>
#include "types/EqTypes.h"

namespace pg {

// EQ gráfica 31 bandas 1/3 octava.
// gainDb (-12..+12) escalado por intensity (0..1) y masterIntensity.
// effectiveGain = gainDb * intensity * masterIntensity.
class Eq31BandProcessor {
public:
  void prepare(double sr, int ch, int blockSize);
  void setState(const Eq31State &s);
  void process(juce::AudioBuffer<float> &buf);

private:
  Eq31State state{};
  std::array<juce::dsp::IIR::Filter<float>, 31> filters;
  double sampleRate = 44100.0;
  juce::SpinLock lock; // UI escribe coeficientes, audio los lee
  void updateCoefficients();
};

} // namespace pg
