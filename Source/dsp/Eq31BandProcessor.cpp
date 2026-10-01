#include "dsp/Eq31BandProcessor.h"

namespace pg {

namespace {
constexpr float kQ = 4.3f;        // ~1/3 octava
constexpr float kMuteDb = -60.0f; // banda muteada: muesca profunda
} // namespace

void Eq31BandProcessor::prepare(double sr, int ch, int blockSize) {
  const juce::SpinLock::ScopedLockType sl(lock);
  sampleRate = sr;
  juce::dsp::ProcessSpec spec{sr, (juce::uint32)blockSize,
                              (juce::uint32)ch};
  for (auto &f : filters)
    f.prepare(spec);
  updateCoefficients();
}

void Eq31BandProcessor::setState(const Eq31State &s) {
  const juce::SpinLock::ScopedLockType sl(lock);
  state = s;
  updateCoefficients();
}

void Eq31BandProcessor::updateCoefficients() {
  for (size_t i = 0; i < 31; ++i) {
    const auto &b = state.bands[i];
    // Mute -> notch; si no, gain escalado por intensidad y master.
    float eff = b.enabled ? b.gainDb * b.intensity * state.masterIntensity
                          : kMuteDb;
    if (state.bypass)
      eff = 0.0f;
    auto coef = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, kEq31Freqs[i], kQ, juce::Decibels::decibelsToGain(eff));
    filters[i].coefficients = coef;
  }
}

void Eq31BandProcessor::process(juce::AudioBuffer<float> &buf) {
  // TryLock: si la UI está actualizando, saltamos este bloque (no bloquea audio).
  const juce::SpinLock::ScopedTryLockType sl(lock);
  if (!sl.isLocked() || buf.getNumChannels() <= 0 || buf.getNumSamples() <= 0)
    return;
  juce::dsp::AudioBlock<float> block(buf);
  juce::dsp::ProcessContextReplacing<float> ctx(block);
  for (auto &f : filters)
    f.process(ctx);
}

} // namespace pg
