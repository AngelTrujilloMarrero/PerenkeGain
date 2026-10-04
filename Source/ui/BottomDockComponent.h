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

  // EQ (~232 px) + nivelador (84).
  int preferredHeight() const { return 316; }

  // Refresco de medidores del EQ y del nivelador; lo llama MeterClock.
  void tickMeters();

  EqualizerComponent eq;
  LevelerPanel leveler;
};

} // namespace pg
