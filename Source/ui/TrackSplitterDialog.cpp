#include "ui/TrackSplitterDialog.h"

namespace pg {

TrackSplitterDialog::TrackSplitterDialog() {
  threshold.setRange(-60.0, -45.0, 0.5);
  threshold.setValue(-50.0);
  minSilence.setRange(0.5, 5.0, 0.1);
  minSilence.setValue(2.0);
  minTrack.setRange(5.0, 120.0, 1.0);
  minTrack.setValue(30.0);
  middle.setToggleState(true, juce::dontSendNotification);
  addAndMakeVisible(threshold);
  addAndMakeVisible(minSilence);
  addAndMakeVisible(minTrack);
  addAndMakeVisible(middle);
}

SilenceParams TrackSplitterDialog::getParams() const {
  return {(float)threshold.getValue(), (float)minSilence.getValue(), 0.25f,
          (float)minTrack.getValue(), middle.getToggleState()};
}

void TrackSplitterDialog::resized() {
  auto r = getLocalBounds();
  threshold.setBounds(r.removeFromTop(40).reduced(4));
  minSilence.setBounds(r.removeFromTop(40).reduced(4));
  minTrack.setBounds(r.removeFromTop(40).reduced(4));
  middle.setBounds(r.removeFromTop(30).reduced(4));
}

} // namespace pg
