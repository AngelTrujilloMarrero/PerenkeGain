#pragma once
#include <JuceHeader.h>
#include "dsp/RumbleFilter.h"
#include "dsp/DeClicker.h"
#include "dsp/DeHiss.h"
#include "dsp/Eq31BandProcessor.h"

namespace pg {

// Orden estricto: HP -> DeClick -> DeHiss -> EQ31.
class DspChain {
public:
  void prepare(double sr, int ch);
  void process(juce::AudioBuffer<float> &buf);
  void setBypass(bool b) { bypassAll = b; }

  RumbleFilter rumble;
  DeClicker declick;
  DeHiss dehiss;
  Eq31BandProcessor eq31;

private:
  bool bypassAll = false;
};

} // namespace pg
