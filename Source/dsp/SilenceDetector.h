#pragma once
#include "types/AudioTypes.h"
#include <vector>

namespace pg {

// Energía RMS por trama: E(k)=10*log10(mean(x^2)). Clasifica silencio.
class SilenceDetector {
public:
  explicit SilenceDetector(SilenceParams p);
  std::vector<TrackRegion> detect(const float *mono, int n,
                                  double sampleRate) const;

private:
  SilenceParams params;
};

} // namespace pg
