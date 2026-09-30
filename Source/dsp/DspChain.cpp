#include "dsp/DspChain.h"

namespace pg {

void DspChain::prepare(double sr, int ch) {
  rumble.prepare(sr, ch);
  eq31.prepare(sr, ch, 512);
}

void DspChain::process(juce::AudioBuffer<float> &buf) {
  if (bypassAll)
    return;
  rumble.process(buf);
  declick.process(buf);
  dehiss.process(buf, 44100.0);
  eq31.process(buf);
}

} // namespace pg
