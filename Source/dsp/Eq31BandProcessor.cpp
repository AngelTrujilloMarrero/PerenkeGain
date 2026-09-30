#include "dsp/Eq31BandProcessor.h"

namespace pg {

void Eq31BandProcessor::prepare(double sr, int ch, int blockSize) {
  sampleRate = sr;
  juce::dsp::ProcessSpec spec{sr, (juce::uint32)blockSize,
                              (juce::uint32)ch};
  for (auto &f : filters)
    f.prepare(spec);
  updateCoefficients();
}

void Eq31BandProcessor::setState(const Eq31State &s) {
  state = s;
  updateCoefficients();
}

void Eq31BandProcessor::updateCoefficients() {
  constexpr float Q = 4.3f; // ~1/3 octava
  for (size_t i = 0; i < 31; ++i) {
    const auto &b = state.bands[i];
    float eff = b.enabled ? b.gainDb * b.intensity * state.masterIntensity
                          : 0.0f;
    if (state.bypass)
      eff = 0.0f;
    auto coef = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, kEq31Freqs[i], Q, juce::Decibels::decibelsToGain(eff));
    *filters[i].coefficients = *coef;
  }
}

void Eq31BandProcessor::process(juce::AudioBuffer<float> &buf) {
  juce::dsp::AudioBlock<float> block(buf);
  juce::dsp::ProcessContextReplacing<float> ctx(block);
  for (auto &f : filters)
    f.process(ctx);
}

} // namespace pg
