#pragma once
#include <JuceHeader.h>

namespace pg {

// Barra de posición cian del editor original (relleno sobre canal blanco).
class CyanProgressBar : public juce::Component {
public:
  void setFraction(double f);
  void paint(juce::Graphics &g) override;

private:
  double fraction = 0.0;
};

} // namespace pg
