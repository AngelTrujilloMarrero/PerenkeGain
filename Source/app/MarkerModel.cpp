#include "app/MarkerModel.h"
#include <algorithm>

namespace pg {

void MarkerModel::setFile(const juce::File &f) {
  if (current == f)
    return;
  current = f;
  cuts.clear();
  sendChangeMessage();
}

void MarkerModel::clear() {
  if (cuts.empty())
    return;
  cuts.clear();
  sendChangeMessage();
}

void MarkerModel::addMarker(double a, double b) {
  cuts.push_back({a, b, (int)cuts.size()});
  sendChangeMessage();
}

void MarkerModel::setMarkers(std::vector<TrackRegion> m) {
  cuts = std::move(m);
  sendChangeMessage();
}

std::vector<TrackRegion>
MarkerModel::segmentsFromCuts(std::vector<double> cuts, double total) {
  cuts.push_back(0.0);
  cuts.push_back(total);
  std::sort(cuts.begin(), cuts.end());
  std::vector<double> uniq;
  for (double c : cuts) {
    c = juce::jlimit(0.0, total, c);
    if (uniq.empty() || c - uniq.back() > 0.05)
      uniq.push_back(c);
  }
  std::vector<TrackRegion> segs;
  for (size_t i = 0; i + 1 < uniq.size(); ++i)
    segs.push_back({uniq[i], uniq[i + 1], (int)i});
  return segs;
}

} // namespace pg
