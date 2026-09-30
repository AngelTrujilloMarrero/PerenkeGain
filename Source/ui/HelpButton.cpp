#include "ui/HelpButton.h"

namespace pg {

HelpButton::HelpButton() : juce::Button("ayuda") {}

void HelpButton::paintButton(juce::Graphics &g, bool highlighted, bool down) {
  auto r = getLocalBounds().toFloat().reduced(3.0f);
  juce::Colour c = down ? juce::Colour(0xFF0000A0)
                        : (highlighted ? juce::Colour(0xFF3060FF)
                                       : juce::Colour(0xFF0000E0));
  g.setColour(c);
  g.fillEllipse(r);
  g.setColour(juce::Colours::black);
  g.drawEllipse(r, 1.5f);
  g.setColour(juce::Colours::white);
  g.setFont(juce::Font(juce::FontOptions(20.0f, juce::Font::bold)));
  g.drawText("?", getLocalBounds(), juce::Justification::centred);
}

} // namespace pg
