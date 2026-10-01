#pragma once
#include <JuceHeader.h>

namespace pg {

// Limitador de seguridad true-peak: evita que la compensacion del nivelador
// recorte. Envoltorio fino sobre juce::dsp::Limiter (techo en dB, clip a 0).
class TruePeakLimiter {
public:
  void prepare(double sampleRate, int numChannels, int blockSize);
  void reset();
  void setCeilingDb(float db);
  void process(juce::AudioBuffer<float> &buf);

private:
  juce::dsp::Limiter<float> limiter;
  float ceilingDb = -1.0f;
};

inline void TruePeakLimiter::prepare(double sampleRate, int numChannels,
                                     int blockSize) {
  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
  spec.maximumBlockSize = (juce::uint32)juce::jmax(1, blockSize);
  spec.numChannels = (juce::uint32)juce::jmax(1, numChannels);
  limiter.prepare(spec);
  limiter.setRelease(50.0f);
  limiter.setThreshold(ceilingDb);
  limiter.reset();
}

inline void TruePeakLimiter::reset() { limiter.reset(); }

inline void TruePeakLimiter::setCeilingDb(float db) {
  if (db == ceilingDb)
    return;
  ceilingDb = db;
  limiter.setThreshold(db);
}

inline void TruePeakLimiter::process(juce::AudioBuffer<float> &buf) {
  juce::dsp::AudioBlock<float> block(buf);
  limiter.process(juce::dsp::ProcessContextReplacing<float>(block));
}

} // namespace pg
