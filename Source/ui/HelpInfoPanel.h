#pragma once
#include <JuceHeader.h>

namespace pg {

// Panel informativo inferior (caja blanca + "Continuar") del original.
class HelpInfoPanel : public juce::Component {
public:
  HelpInfoPanel();
  void resized() override;
  bool isTipVisible() const { return tip.getToggleState(); }
  std::function<void()> onContinue;

private:
  juce::Label text;
  juce::ToggleButton tip{"No mostrarme ya este mensaje"};
  juce::TextButton cont{"Continuar"};
};

} // namespace pg
