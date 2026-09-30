#pragma once
#include <JuceHeader.h>
#include "types/AudioTypes.h"

namespace pg {

// Análisis en background (jthread) para no bloquear UI.
class TrackSplitterService {
public:
  std::vector<TrackRegion> analyze(const juce::AudioBuffer<float> &buf,
                                   double sr, SilenceParams p);
};

} // namespace pg
