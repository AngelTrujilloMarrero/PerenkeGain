#pragma once
#include <JuceHeader.h>

namespace pg {

// Ventana "Acerca de": portada del logo + nombre, version y licencia.
class AboutDialog : public juce::Component {
public:
  AboutDialog();
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  juce::Image hero;
  juce::HyperlinkButton repo;
};

} // namespace pg
