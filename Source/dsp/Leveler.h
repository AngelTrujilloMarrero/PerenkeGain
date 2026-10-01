#pragma once
#include <JuceHeader.h>
#include <atomic>
#include "types/NormalizeTypes.h"
#include "dsp/LoudnessMeterRT.h"
#include "dsp/TruePeakLimiter.h"

namespace pg {

// Nivelador dinamico en vivo: mide la sonoridad (LUFS), calcula la ganancia
// para acercarla al objetivo y la aplica con ballistics (attack/release),
// respetando el gate y el techo true-peak. No persiste ni exporta.
class Leveler {
public:
  void prepare(double sampleRate, int numChannels, int blockSize);
  void reset();
  void setParams(const LevelerParams &p);
  LevelerParams getParams() const;
  void process(juce::AudioBuffer<float> &buf);
  LevelerMeters getMeters() const;

private:
  LoudnessMeterRT meter;
  TruePeakLimiter limiter;
  double rate = 44100.0;
  float currentGainDb = 0.0f;
  bool wasEnabled = false;
  LevelerParams cached{};
  LevelerParams params;
  mutable juce::SpinLock lock;
  std::atomic<float> appliedGainDb{0.0f};
};

} // namespace pg
