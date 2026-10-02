#pragma once
#include <JuceHeader.h>

namespace pg {

// LookAndFeel oscuro moderno: botones planos redondeados con acento cian,
// checkboxes minimalistas y sliders con pista fina. Sustituye al gris
// biselado estilo Windows 9x.
class ModernLookAndFeel : public juce::LookAndFeel_V4 {
public:
  ModernLookAndFeel();

  void drawButtonBackground(juce::Graphics &g, juce::Button &b,
                            const juce::Colour &bg, bool highlighted,
                            bool down) override;
  void drawToggleButton(juce::Graphics &g, juce::ToggleButton &b,
                        bool highlighted, bool down) override;
  void drawComboBox(juce::Graphics &g, int width, int height, bool down,
                    int buttonX, int buttonY, int buttonW, int buttonH,
                    juce::ComboBox &box) override;
  void drawLinearSlider(juce::Graphics &g, int x, int y, int width,
                        int height, float sliderPos, float, float,
                        const juce::Slider::SliderStyle, juce::Slider &s)
      override;
};

// Fondos y marcos compartidos por los paneles oscuros.
namespace modern {
juce::Colour surface();
juce::Colour card();
juce::Colour accent();
void fillBackground(juce::Graphics &g, juce::Rectangle<int> r);
void frame(juce::Graphics &g, juce::Rectangle<int> r);
} // namespace modern

} // namespace pg
