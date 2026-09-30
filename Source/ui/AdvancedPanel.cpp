#include "ui/AdvancedPanel.h"
#include "types/Text.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

AdvancedPanel::AdvancedPanel() {
  for (auto *t : {&click, &noise, &eq, &ab})
    addAndMakeVisible(*t);
  addAndMakeVisible(reset);
  ab.setToggleState(true, juce::dontSendNotification);
}

void AdvancedPanel::paint(juce::Graphics &g) {
  g.setColour(retro::face());
  g.fillAll();
  g.setColour(juce::Colours::black);
  g.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
  g.drawText(PG_T("Filtros de restauración"), getLocalBounds().removeFromTop(22),
             juce::Justification::centred);
}

void AdvancedPanel::resized() {
  auto r = getLocalBounds().reduced(8);
  r.removeFromTop(22);
  int h = 26;
  for (auto *t : {&click, &noise, &eq, &ab})
    t->setBounds(r.removeFromTop(h));
  r.removeFromTop(8);
  reset.setBounds(r.removeFromTop(28).reduced(14, 0));
}

} // namespace pg
