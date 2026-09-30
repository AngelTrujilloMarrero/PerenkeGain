#pragma once
#include <JuceHeader.h>
#include "types/AudioTypes.h"

namespace pg {

// Export offline 32-float a WAV/MP3/FLAC por regiones.
class ExportManager {
public:
  bool exportTracks(const juce::AudioBuffer<float> &buf, double sr,
                    const std::vector<TrackRegion> &regions,
                    const juce::File &dir, int formatId);
};

} // namespace pg
