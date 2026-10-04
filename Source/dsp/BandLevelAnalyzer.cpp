#include "dsp/BandLevelAnalyzer.h"
#include <cmath>

namespace pg {

void BandLevelAnalyzer::prepare(double sampleRate) {
  for (auto &m : masterLv)
    m.store(-60.0f, std::memory_order_relaxed);
  if (rate == sampleRate)
    return;
  rate = sampleRate;
  for (int b = 0; b < kBands; ++b) {
    // Banda de 1/3 de octava: Q ≈ 1 / (2^(1/6) - 2^(-1/6)) ≈ 4.3
    for (int c = 0; c < 2; ++c) {
      auto &f = filters[(size_t)c][(size_t)b];
      f.reset();
      f.setBandPass(sampleRate, kEq31Freqs[(size_t)b], 4.3);
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
  std::array<double, 2> masterAcc{0.0, 0.0};

  for (int c = 0; c < chans; ++c) {
    if (in[c] == nullptr)
      continue;
    double rawSum = 0.0;
    for (int i = 0; i < numSamples; ++i) {
      double s = (double)in[c][i];
      rawSum += s * s;
    }
    masterAcc[(size_t)c] = rawSum / (double)numSamples;
    for (int b = 0; b < kBands; ++b) {
      auto &f = filters[(size_t)c][(size_t)b];
      double sumSq = 0.0;
      for (int i = 0; i < numSamples; ++i) {
        double s = (double)f.process(in[c][i]);
        sumSq += s * s; // energía (RMS), no media de la señal
      }
      acc[(size_t)b] += sumSq / (double)numSamples;
    }
  }

  for (int b = 0; b < kBands; ++b) {
    float rms = (float)std::sqrt(acc[(size_t)b] / (double)chans);
    float db = juce::jmax(-60.0f, 20.0f * std::log10(rms + 1.0e-6f));
    float prev = levels[(size_t)b].load(std::memory_order_relaxed);
    float next = db > prev ? db : juce::jmax(-60.0f, prev - decay);
    levels[(size_t)b].store(next, std::memory_order_relaxed);
  }

  for (int c = 0; c < 2; ++c) {
    float mRms = (float)std::sqrt(masterAcc[(size_t)c]);
    float mDb = juce::jmax(-60.0f, 20.0f * std::log10(mRms + 1.0e-6f));
    float prevM = masterLv[(size_t)c].load(std::memory_order_relaxed);
    float nextM = mDb > prevM ? mDb : juce::jmax(-60.0f, prevM - decay);
    masterLv[(size_t)c].store(nextM, std::memory_order_relaxed);
  }
}

void BandLevelAnalyzer::processSilence(int numSamples) {
  if (rate <= 0.0 || numSamples <= 0)
    return;
  const float decay = 45.0f * (float)numSamples / (float)rate;
  for (auto &l : levels) {
    const float prev = l.load(std::memory_order_relaxed);
    l.store(juce::jmax(-60.0f, prev - decay), std::memory_order_relaxed);
  }
  for (auto &m : masterLv)
    m.store(-60.0f, std::memory_order_relaxed);
  // Limpia el estado de los filtros para no arrastrarlo al reanudar.
  for (auto &ch : filters)
    for (auto &f : ch)
      f.reset();
}

float BandLevelAnalyzer::masterLevelDb(int channel) const {
  if (channel < 0 || channel >= 2)
    return -60.0f;
  return juce::jlimit(
      -60.0f, 0.0f, masterLv[(size_t)channel].load(std::memory_order_relaxed));
}

float BandLevelAnalyzer::levelDb(int band) const {
  if (band < 0 || band >= kBands)
    return -60.0f;
  return juce::jlimit(
      -60.0f, 0.0f,
      levels[(size_t)band].load(std::memory_order_relaxed));
}

} // namespace pg
