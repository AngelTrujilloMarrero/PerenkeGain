#include "dsp/BandLevelAnalyzer.h"
#include <cmath>

namespace pg {

void BandLevelAnalyzer::prepare(double sampleRate) {
  if (rate == sampleRate)
    return;
  rate = sampleRate;
  juce::dsp::ProcessSpec spec{sampleRate, 512, 1}; // un estado por filtro
  for (int b = 0; b < kBands; ++b) {
    // Banda de 1/3 de octava: Q ≈ 1 / (2^(1/6) - 2^(-1/6)) ≈ 4.3
    auto coef = juce::dsp::IIR::Coefficients<float>::makeBandPass(
        sampleRate, kEq31Freqs[(size_t)b], 4.3);
    for (int c = 0; c < 2; ++c) {
      auto &f = filters[(size_t)c][(size_t)b];
      f.coefficients = coef;
      f.prepare(spec);
    }
    levels[(size_t)b].store(-60.0f, std::memory_order_relaxed);
  }
}

void BandLevelAnalyzer::process(const float *const *in, int numChannels,
                                int numSamples) {
  if (rate <= 0.0 || numSamples <= 0)
    return;
  int chans = juce::jlimit(1, 2, numChannels);
  // Caída ~45 dB/s entre bloques (tipo VU con liberación)
  float decay = 45.0f * (float)numSamples / (float)rate;
  std::array<double, kBands> acc{};

  for (int c = 0; c < chans; ++c) {
    if (in[c] == nullptr)
      continue;
    for (int b = 0; b < kBands; ++b) {
      auto &f = filters[(size_t)c][(size_t)b];
      double sum = 0.0;
      for (int i = 0; i < numSamples; ++i)
        sum += (double)f.processSample(in[c][i]);
      acc[(size_t)b] += sum / (double)numSamples;
    }
  }

  for (int b = 0; b < kBands; ++b) {
    float rms = (float)std::sqrt(acc[(size_t)b] / (double)chans);
    float db = juce::jmax(-60.0f, 20.0f * std::log10(rms + 1.0e-6f));
    float prev = levels[(size_t)b].load(std::memory_order_relaxed);
    float next = db > prev ? db : juce::jmax(-60.0f, prev - decay);
    levels[(size_t)b].store(next, std::memory_order_relaxed);
  }
}

float BandLevelAnalyzer::levelDb(int band) const {
  if (band < 0 || band >= kBands)
    return -60.0f;
  return juce::jlimit(
      -60.0f, 0.0f,
      levels[(size_t)band].load(std::memory_order_relaxed));
}

} // namespace pg
