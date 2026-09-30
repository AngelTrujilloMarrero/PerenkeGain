#include "audio/TrackMarkersComponent.h"

namespace pg {

void TrackMarkersComponent::setRegions(const std::vector<TrackRegion> &r) {
  regions = r;
  if (!regions.empty())
    totalSec = regions.back().endSec;
  repaint();
}

void TrackMarkersComponent::paint(juce::Graphics &g) {
  g.fillAll(juce::Colours::darkgrey.darker(0.6f));
  g.setColour(juce::Colours::yellow);
  for (auto &tr : regions) {
    int x = int(tr.startSec / totalSec * getWidth());
    g.drawLine((float)x, 0.0f, (float)x, (float)getHeight(), 2.0f);
  }
}

} // namespace pg
