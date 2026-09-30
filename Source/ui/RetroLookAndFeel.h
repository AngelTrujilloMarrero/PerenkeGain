#pragma once
#include <JuceHeader.h>

namespace pg {

// LookAndFeel estilo Windows 9x/2000: gris #D4D0C8, bordes 3D,
// checkbox clásico, barra cian. Basado en capturas del PolderbitS original.
class RetroLookAndFeel : public juce::LookAndFeel_V4 {
public:
  RetroLookAndFeel();

  void drawButtonBackground(juce::Graphics &g, juce::Button &b,
                            const juce::Colour &bg, bool highlighted,
                            bool down) override;
  void drawToggleButton(juce::Graphics &g, juce::ToggleButton &b,
                        bool highlighted, bool down) override;
  void drawLinearSlider(juce::Graphics &g, int x, int y, int width,
                        int height, float sliderPos, float, float,
                        const juce::Slider::SliderStyle, juce::Slider &s)
      override;
};

// Utilidades de bisel compartidas por componentes retro.
namespace retro {
juce::Colour face();
void bevelRaised(juce::Graphics &g, juce::Rectangle<int> r);
void bevelSunken(juce::Graphics &g, juce::Rectangle<int> r);
} // namespace retro

} // namespace pg
