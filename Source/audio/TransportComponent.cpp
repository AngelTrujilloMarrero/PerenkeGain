#include "audio/TransportComponent.h"

namespace pg {

TransportComponent::TransportComponent() {
  for (auto *b : {&play, &stop, &toEnd}) {
    b->setTooltip(b->getButtonText());
    addAndMakeVisible(*b);
  }
}

void TransportComponent::resized() {
  auto r = getLocalBounds();
  int s = juce::jmin(r.getHeight(), 30);
  for (auto *b : {&play, &stop, &toEnd})
    b->setBounds(r.removeFromLeft(s).reduced(1));
}

} // namespace pg
