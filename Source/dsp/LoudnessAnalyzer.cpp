#include "dsp/LoudnessAnalyzer.h"
#include "dsp/IntegratedLoudness.h"

namespace pg {

float LoudnessAnalyzer::rmsDbF(const juce::AudioBuffer<float> &buf,
                               double sampleRate) {
  return IntegratedLoudness::analyze(buf, sampleRate);
}

} // namespace pg
