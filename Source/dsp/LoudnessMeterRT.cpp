#include "dsp/LoudnessMeterRT.h"
#include <cmath>

namespace pg {

void LoudnessMeterRT::prepare(double sampleRate, int numChannels) {
  rate = sampleRate > 0.0 ? sampleRate : 44100.0;
  channels = juce::jmax(1, numChannels);
  hopSamples = juce::jmax(1, (int)std::round(rate * 0.1));
  kFilter.prepare(rate, channels);
  hopSumSq.assign((size_t)channels, 0.0);
  ring.assign((size_t)channels, {});
  reset();
}

void LoudnessMeterRT::reset() {
  kFilter.reset();
  hopFilled = 0;
  hopCount = 0;
  hopIndex = 0;
  blockPeak = 0.0f;
  for (auto &ch : ring)
    ch.fill(0.0);
  for (auto &v : hopSumSq)
    v = 0.0;
  momentary.store(-70.0f);
  shortTerm.store(-70.0f);
  peak.store(-70.0f);
}

void LoudnessMeterRT::process(const juce::AudioBuffer<float> &buf) {
  const int chans = juce::jmin(channels, buf.getNumChannels());
  const int n = buf.getNumSamples();
  for (int i = 0; i < n; ++i) {
    for (int ch = 0; ch < chans; ++ch) {
      const float x = buf.getReadPointer(ch)[i];
      blockPeak = juce::jmax(blockPeak, std::abs(x));
      const float y = kFilter.processSample(ch, x);
      hopSumSq[(size_t)ch] += (double)y * (double)y;
    }
    if (++hopFilled >= hopSamples)
      flushHop();
  }
}

void LoudnessMeterRT::flushHop() {
  for (int ch = 0; ch < channels; ++ch) {
    ring[(size_t)ch][(size_t)hopIndex] =
        hopSumSq[(size_t)ch] / (double)hopSamples;
    hopSumSq[(size_t)ch] = 0.0;
  }
  hopIndex = (hopIndex + 1) % kHops;
  hopCount = juce::jmin(hopCount + 1, kHops);
  hopFilled = 0;

  momentary.store(juce::jmax(-70.0f, windowLufs(kMomentaryHops)));
  shortTerm.store(juce::jmax(-70.0f, windowLufs(kHops)));
  peak.store(juce::jmax(
      -70.0f, juce::Decibels::gainToDecibels(blockPeak, -70.0f)));
  blockPeak = 0.0f;
}

// Suma, por canal, la media de los ultimos 'hops' bloques; el resultado es la
// potencia media ponderada de la ventana y se pasa a LUFS (BS.1770).
float LoudnessMeterRT::windowLufs(int hops) const {
  hops = juce::jmin(hops, hopCount);
  if (hops <= 0)
    return -100.0f;
  double sum = 0.0;
  for (int h = 0; h < hops; ++h) {
    const int idx = (hopIndex - 1 - h + kHops * 2) % kHops;
    for (int ch = 0; ch < channels; ++ch)
      sum += ring[(size_t)ch][(size_t)idx];
  }
  sum /= (double)hops;
  if (sum <= 1.0e-12)
    return -100.0f;
  return (float)(-0.691 + 10.0 * std::log10(sum));
}

} // namespace pg
