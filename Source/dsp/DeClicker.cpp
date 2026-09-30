#include "dsp/DeClicker.h"
#include <cmath>

namespace pg {

void DeClicker::process(juce::AudioBuffer<float> &buf) {
  for (int ch = 0; ch < buf.getNumChannels(); ++ch) {
    auto *d = buf.getWritePointer(ch);
    int n = buf.getNumSamples();
    // Media/desv de derivada en ventana simple global.
    double mean = 0.0;
    for (int i = 1; i < n; ++i)
      mean += std::abs(d[i] - d[i - 1]);
    mean /= n;
    double var = 0.0;
    for (int i = 1; i < n; ++i) {
      double v = std::abs(d[i] - d[i - 1]) - mean;
      var += v * v;
    }
    double sd = std::sqrt(var / n);
    float thr = (float)(mean + sensitivityK * sd);
    for (int i = 1; i < n - 1; ++i) {
      if (std::abs(d[i] - d[i - 1]) > thr)
        d[i] = 0.5f * (d[i - 1] + d[i + 1]); // interpola lineal (MVP)
    }
  }
}

} // namespace pg
