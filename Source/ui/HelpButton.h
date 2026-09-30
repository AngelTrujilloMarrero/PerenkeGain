#pragma once
#include <JuceHeader.h>

namespace pg {

// Botón circular azul de ayuda "?" del editor original.
class HelpButton : public juce::Button {
public:
  HelpButton();
  void paintButton(juce::Graphics &g, bool highlighted, bool down) override;
};

} // namespace pg
