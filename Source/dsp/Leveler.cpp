#include "dsp/Leveler.h"
#include <cmath>

namespace pg {

void Leveler::prepare(double sampleRate, int numChannels, int blockSize) {
  rate = sampleRate > 0.0 ? sampleRate : 44100.0;
  meter.prepare(rate, numChannels);
  limiter.prepare(rate, numChannels, blockSize);
  reset();
}

void Leveler::reset() {
  meter.reset();
  limiter.reset();
  currentGainDb = 0.0f;
  wasEnabled = false;
  appliedGainDb.store(0.0f);
}

void Leveler::setParams(const LevelerParams &p) {
  const juce::SpinLock::ScopedLockType sl(lock);
  params = p;
}

LevelerParams Leveler::getParams() const {
  const juce::SpinLock::ScopedLockType sl(lock);
  return params;
}

void Leveler::process(juce::AudioBuffer<float> &buf) {
  {
    // Try-lock: el hilo de audio nunca se bloquea; usa la ultima config.
    const juce::SpinLock::ScopedTryLockType sl(lock);
    if (sl.isLocked())
      cached = params;
  }

  if (!cached.enabled) {
    if (wasEnabled) {
      meter.reset();
      limiter.reset();
      currentGainDb = 0.0f;
      appliedGainDb.store(0.0f);
    }
    wasEnabled = false;
    return;
  }
  wasEnabled = true;

  meter.process(buf);
  const float loud = meter.shortTermLufs();

  float desired = currentGainDb; // congelado en silencio (gate)
  if (loud > cached.gateLufs) {
    desired = juce::jlimit(-cached.maxCutDb, cached.maxBoostDb,
                           cached.targetLufs - loud);
  }

  const float blockSec = (float)buf.getNumSamples() / (float)rate;
  const float tau =
      desired < currentGainDb ? cached.attackSec : cached.releaseSec;
  float coeff = tau > 0.0f ? 1.0f - std::exp(-blockSec / tau) : 1.0f;
  coeff = juce::jlimit(0.0f, 1.0f, coeff);
  currentGainDb += (desired - currentGainDb) * coeff;

  buf.applyGain(juce::Decibels::decibelsToGain(currentGainDb));
  limiter.setCeilingDb(cached.ceilingDb);
  limiter.process(buf);
  appliedGainDb.store(currentGainDb);
}

LevelerMeters Leveler::getMeters() const {
  LevelerMeters m;
  m.momentaryLufs = meter.momentaryLufs();
  m.shortTermLufs = meter.shortTermLufs();
  m.appliedGainDb = appliedGainDb.load();
  m.peakDb = meter.peakDb();
  return m;
}

} // namespace pg
