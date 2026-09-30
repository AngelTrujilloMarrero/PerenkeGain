#pragma once
#include <JuceHeader.h>

namespace pg {

// Panel Advanced: toggles Click/Crackle/Noise/EQ + A/B original.
class AdvancedPanel : public juce::Component {
public:
  AdvancedPanel();
  void resized() override;

private:
  juce::ToggleButton click{"Click/Crackle vinilo"}, noise{"Noise casete"},
      eq{"EQ 31 bandas"}, ab{"Escuchar filtrado (A/B)"};
};

} // namespace pg
