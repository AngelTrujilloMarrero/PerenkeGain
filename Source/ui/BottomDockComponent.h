#pragma once
#include <JuceHeader.h>
#include "ui/EqualizerComponent.h"
#include "ui/LevelerPanel.h"

namespace pg {

// Bandeja inferior fija del editor: ecualizador con el nivelador de
// sonoridad justo debajo. Siempre visible.
class BottomDockComponent : public juce::Component {
public:
  BottomDockComponent();
  void paint(juce::Graphics &g) override;
  void resized() override;

  int preferredHeight() const { return 510; }

  EqualizerComponent eq;
  LevelerPanel leveler;
};

} // namespace pg
