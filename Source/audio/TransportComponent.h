#pragma once
#include <JuceHeader.h>

namespace pg {

// Transporte estilo pletina: Play/Stop + Cut-Start/End + Fade.
class TransportComponent : public juce::Component {
public:
  TransportComponent();
  void resized() override;
  juce::TextButton play{"Play"}, stop{"Stop"}, trim{"Trim"}, fadeIn{"Fade-In"},
      fadeOut{"Fade-Out"};

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};

} // namespace pg
