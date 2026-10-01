#pragma once
#include <JuceHeader.h>
#include <array>

namespace pg {

// Filtro K de ITU-R BS.1770: high-shelf (+4 dB ~1.68 kHz) seguido de
// high-pass (~38 Hz). Modela la ponderacion psicoacustica usada para
// medir sonoridad (LUFS). Un filtro por canal.
class KWeightingFilter {
public:
  static constexpr int kMaxChannels = 2;

  void prepare(double sampleRate, int numChannels);
  void reset();
  float processSample(int channel, float sample);

private:
  using Filter = juce::dsp::IIR::Filter<float>;
  std::array<Filter, kMaxChannels> shelf, highpass;
  int active = 0;
};

inline void KWeightingFilter::prepare(double sampleRate, int numChannels) {
  active = juce::jlimit(1, kMaxChannels, numChannels);
  auto shelfCoef = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
      sampleRate, 1681.97, 0.7071, juce::Decibels::decibelsToGain(4.0f));
  auto hpCoef = juce::dsp::IIR::Coefficients<float>::makeHighPass(
      sampleRate, 38.13, 0.5f);
  for (int i = 0; i < kMaxChannels; ++i) {
    shelf[(size_t)i].coefficients = shelfCoef;
    shelf[(size_t)i].reset();
    highpass[(size_t)i].coefficients = hpCoef;
    highpass[(size_t)i].reset();
  }
}

inline void KWeightingFilter::reset() {
  for (auto &f : shelf)
    f.reset();
  for (auto &f : highpass)
    f.reset();
}

inline float KWeightingFilter::processSample(int channel, float sample) {
  if (channel < 0 || channel >= active)
    return sample;
  return highpass[(size_t)channel].processSample(
      shelf[(size_t)channel].processSample(sample));
}

} // namespace pg
