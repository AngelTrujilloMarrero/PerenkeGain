#pragma once
#include <JuceHeader.h>

namespace pg {

// Paso alto Butterworth 20 Hz: rumble vinilo + DC offset.
class RumbleFilter {
public:
  void prepare(double sr, int ch);
  void process(juce::AudioBuffer<float> &buf);

private:
  juce::dsp::ProcessorChain<juce::dsp::IIR::Filter<float>> chain;
};

} // namespace pg
