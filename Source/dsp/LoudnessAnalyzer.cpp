#include "dsp/LoudnessAnalyzer.h"
#include <cmath>

namespace pg {

float LoudnessAnalyzer::rmsDbF(const juce::AudioBuffer<float> &buf,
                               double sampleRate) {
  const int nch = buf.getNumChannels();
  const int n = buf.getNumSamples();
  if (nch <= 0 || n <= 0)
    return -100.0f;

  KWeightingFilter k;
  k.prepare(sampleRate, nch);

  double sumSq = 0.0;
  for (int i = 0; i < n; ++i) {
    double mix = 0.0;
    for (int ch = 0; ch < nch; ++ch)
      mix += k.processSample(ch, buf.getReadPointer(ch)[i]);
    mix /= (double)nch;
    sumSq += mix * mix;
  }
  const double mean = sumSq / (double)n;
  return mean > 1.0e-12 ? (float)(10.0 * std::log10(mean)) : -100.0f;
}

} // namespace pg
