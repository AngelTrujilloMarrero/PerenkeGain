#pragma once
#include <JuceHeader.h>
#include "types/AudioTypes.h"

namespace pg {

// Barra de marcadores arrastrables sobre el waveform.
class TrackMarkersComponent : public juce::Component {
public:
  void setRegions(const std::vector<TrackRegion> &r);
  void paint(juce::Graphics &g) override;

private:
  std::vector<TrackRegion> regions;
  double totalSec = 1.0;
};

} // namespace pg
