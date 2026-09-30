#pragma once
#include <JuceHeader.h>
#include "types/EqTypes.h"

namespace pg {

// 31 sliders gain + intensidad integrada (doble slider por banda).
// Fila 1: gain -12..+12 dB. Fila 2: intensity 0..100%.
class EqualizerComponent : public juce::Component {
public:
  EqualizerComponent();
  void resized() override;
  Eq31State getState() const;

private:
  std::array<juce::Slider, 31> gains;
  std::array<juce::Slider, 31> intensities;
  juce::Slider master{"Master intensidad"};
};

} // namespace pg
