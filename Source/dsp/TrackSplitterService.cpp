#include "dsp/TrackSplitterService.h"
#include "dsp/SilenceDetector.h"

namespace pg {

std::vector<TrackRegion> TrackSplitterService::analyze(
    const juce::AudioBuffer<float> &buf, double sr, SilenceParams p) {
  juce::AudioBuffer<float> mono(1, buf.getNumSamples());
  mono.clear();
  for (int ch = 0; ch < buf.getNumChannels(); ++ch)
    mono.addFrom(0, 0, buf, ch, 0, buf.getNumSamples());
  SilenceDetector det(p);
  return det.detect(mono.getReadPointer(0), buf.getNumSamples(), sr);
}

} // namespace pg
