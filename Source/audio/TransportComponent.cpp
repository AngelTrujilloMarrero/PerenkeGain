#include "audio/TransportComponent.h"

namespace pg {

TransportComponent::TransportComponent() {
  for (auto *b : {&play, &stop, &trim, &fadeIn, &fadeOut})
    addAndMakeVisible(b);
}

void TransportComponent::resized() {
  auto r = getLocalBounds();
  int w = r.getWidth() / 5;
  play.setBounds(r.removeFromLeft(w).reduced(4));
  stop.setBounds(r.removeFromLeft(w).reduced(4));
  trim.setBounds(r.removeFromLeft(w).reduced(4));
  fadeIn.setBounds(r.removeFromLeft(w).reduced(4));
  fadeOut.setBounds(r.reduced(4));
}

} // namespace pg
