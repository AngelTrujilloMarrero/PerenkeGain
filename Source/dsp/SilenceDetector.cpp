#include "dsp/SilenceDetector.h"
#include <cmath>

namespace pg {

SilenceDetector::SilenceDetector(SilenceParams p) : params(p) {}

std::vector<TrackRegion> SilenceDetector::detect(const float *mono, int n,
                                                 double sr) const {
  const int N = 2048, M = 1024;
  std::vector<TrackRegion> out;
  double silStart = -1.0;
  double trackStart = 0.0;

  for (int k = 0; k * M + N < n; ++k) {
    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
      float x = mono[k * M + i];
      sum += x * x;
    }
    float e = 10.f * std::log10(sum / N + 1e-12f);
    double t = (k * M) / sr;
    bool silent = e < params.thresholdDbFS;

    if (silent && silStart < 0)
      silStart = t;
    if (!silent && silStart >= 0) {
      double dur = t - silStart;
      if (dur >= params.minSilenceSec &&
          silStart - trackStart >= params.minTrackSec) {
        double cut = params.splitInMiddle ? silStart + dur * 0.5 : silStart;
        out.push_back({trackStart, cut, (int)out.size()});
        trackStart = cut;
      }
      silStart = -1.0;
    }
  }
  out.push_back({trackStart, n / sr, (int)out.size()});
  return out;
}

} // namespace pg
