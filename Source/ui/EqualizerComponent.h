#pragma once
#include <JuceHeader.h>
#include "types/EqTypes.h"

namespace pg {

// EQ gráfica 31 bandas estilo PolderbitS Advanced:
// fila superior de ganancia (-12..+12 dB) e inferior de intensidad (0..100).
class EqualizerComponent : public juce::Component {
public:
  EqualizerComponent();
  void paint(juce::Graphics &g) override;
  void resized() override;
  Eq31State getState() const;
  void setState(const Eq31State &s);

private:
  std::array<juce::Slider, 31> gains;
  std::array<juce::Slider, 31> intensities;
  juce::Slider master;
  juce::Label gainTitle, intensityTitle, masterTitle, freqLabelsTitle;
  int colW() const;
};

} // namespace pg
