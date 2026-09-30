#include "ui/AdvancedPanel.h"

namespace pg {

AdvancedPanel::AdvancedPanel() {
  for (auto *t : {&click, &noise, &eq, &ab})
    addAndMakeVisible(t);
  ab.setToggleState(true, juce::dontSendNotification);
}

void AdvancedPanel::resized() {
  auto r = getLocalBounds();
  int h = r.getHeight() / 4;
  for (auto *t : {&click, &noise, &eq, &ab})
    t->setBounds(r.removeFromTop(h).reduced(4));
}

} // namespace pg
