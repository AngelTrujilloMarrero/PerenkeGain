#include "dsp/DeHiss.h"

namespace pg {

void DeHiss::setParams(float a, float b, float) {
  alpha = a;
  beta = b;
}

void DeHiss::process(juce::AudioBuffer<float> &buf, double) {
  // MVP offline simple: gate suave por umbral adaptativo.
  for (int ch = 0; ch < buf.getNumChannels(); ++ch) {
    auto *d = buf.getWritePointer(ch);
    for (int i = 0; i < buf.getNumSamples(); ++i) {
      float mag = std::abs(d[i]);
      float floor = beta * mag;
      float sub = mag - alpha * 0.002f;
      float clean = juce::jmax(sub, floor);
      d[i] = (d[i] >= 0 ? clean : -clean);
    }
  }
}

} // namespace pg
