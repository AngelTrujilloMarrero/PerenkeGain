#include "dsp/Eq31BandProcessor.h"

namespace pg {

namespace {
constexpr float kQ = 4.3f;        // ~1/3 octava
constexpr float kMuteDb = -60.0f; // banda muteada: muesca profunda
} // namespace

void Eq31BandProcessor::prepare(double sr, int ch, int blockSize) {
  juce::ignoreUnused(blockSize);
  const juce::SpinLock::ScopedLockType sl(lock);
  sampleRate = sr > 0.0 ? sr : 44100.0;
  activeChannels = juce::jlimit(1, kMaxChannels, ch);
  for (auto &chFilters : filters)
    for (auto &f : chFilters)
      f.reset();
  updateCoefficients();
}

void Eq31BandProcessor::setState(const Eq31State &s) {
  const juce::SpinLock::ScopedLockType sl(lock);
  state = s;
  updateCoefficients();
}

void Eq31BandProcessor::updateCoefficients() {
  for (auto &chFilters : filters) {
    for (size_t i = 0; i < 31; ++i) {
      const auto &b = state.bands[i];
      // Banda muteada -> muesca; si no, gain escalado por intensidad.
      float eff = b.enabled ? b.gainDb * b.intensity : kMuteDb;
      if (state.bypass)
        eff = 0.0f;
      chFilters[i].setPeak(sampleRate, kEq31Freqs[i], kQ,
                           juce::Decibels::decibelsToGain(eff));
    }
  }
}

void Eq31BandProcessor::resetState() {
  const juce::SpinLock::ScopedTryLockType sl(lock);
  if (!sl.isLocked())
    return;
  for (auto &chFilters : filters)
    for (auto &f : chFilters)
      f.reset();
}

void Eq31BandProcessor::process(juce::AudioBuffer<float> &buf) {
  // TryLock: si la UI está actualizando, saltamos este bloque (no bloquea audio).
  const juce::SpinLock::ScopedTryLockType sl(lock);
  if (!sl.isLocked() || buf.getNumChannels() <= 0 || buf.getNumSamples() <= 0)
    return;
  const float master = juce::jlimit(0.0f, 1.0f, state.masterIntensity);
  const int chans = juce::jmin(activeChannels, buf.getNumChannels());
  const int n = buf.getNumSamples();
  for (int c = 0; c < chans; ++c) {
    auto *d = buf.getWritePointer(c);
    for (int i = 0; i < n; ++i) {
      float x = d[i];
      for (auto &f : filters[(size_t)c])
        x = f.process(x);
      d[i] = x * master;
    }
  }
}

} // namespace pg
