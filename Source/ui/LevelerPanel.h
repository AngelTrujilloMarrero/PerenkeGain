#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"

namespace pg {

// Nivelador de sonoridad en vivo incrustado en la bandeja inferior, debajo
// del ecualizador. Controles compactos + lectura de medidores.
class LevelerPanel : public juce::Component, private juce::Timer {
public:
  LevelerPanel();
  void setEngine(AudioEngine *e);
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  void timerCallback() override;
  void pushParams();
  float speedAttack() const;
  float speedRelease() const;

  AudioEngine *engine = nullptr;
  juce::ToggleButton enable{"Nivelador"};
  juce::ComboBox speed;
  juce::Slider target, boost, cut, ceiling, gate;
  juce::Label targetL, speedL, boostL, cutL, ceilingL, gateL;
  juce::Label momentaryL, shortTermL, gainL, peakL;
};

} // namespace pg
