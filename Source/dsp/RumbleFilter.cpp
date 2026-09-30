#include "dsp/RumbleFilter.h"

namespace pg {

void RumbleFilter::prepare(double sr, int ch) {
  juce::dsp::ProcessSpec spec{sr, 512, (juce::uint32)ch};
  chain.prepare(spec);
  auto coef =
      juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, 20.0f);
  *chain.get<0>().coefficients = *coef;
}

void RumbleFilter::process(juce::AudioBuffer<float> &buf) {
  juce::dsp::AudioBlock<float> block(buf);
  juce::dsp::ProcessContextReplacing<float> ctx(block);
  chain.process(ctx);
}

} // namespace pg
