#pragma once
#include <JuceHeader.h>

namespace pg {

// De-click: d[n]=|x[n]-x[n-1]|, umbral μ+Kσ, interpola cúbica.
class DeClicker {
public:
  void setSensitivity(float k) { sensitivityK = k; }
  void process(juce::AudioBuffer<float> &buf);

private:
  float sensitivityK = 4.0f;
};

} // namespace pg
