#include "ui/BottomDockComponent.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

BottomDockComponent::BottomDockComponent() {
  addAndMakeVisible(eq);
  addAndMakeVisible(leveler);
}

void BottomDockComponent::paint(juce::Graphics &g) {
  g.setColour(juce::Colour(0xFF1A1C22));
  g.fillAll();
}

void BottomDockComponent::resized() {
  auto r = getLocalBounds();
  leveler.setBounds(r.removeFromBottom(84));
  eq.setBounds(r);
}

} // namespace pg
