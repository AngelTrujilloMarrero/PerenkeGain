#pragma once
#include <JuceHeader.h>

namespace pg {

// De-hiss por sustracción espectral STFT/OLA (MVP: one-pole adaptativo).
// |S|=max(|X|-αN, β|X|), α>=1, β~0.01.
class DeHiss {
public:
  void setParams(float alpha, float beta, float noiseLearnSec);
  void process(juce::AudioBuffer<float> &buf, double sr);

private:
  float alpha = 2.0f, beta = 0.01f;
};

} // namespace pg
