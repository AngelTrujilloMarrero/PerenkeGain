#pragma once
#include <JuceHeader.h>

namespace pg {

// Contenido del diálogo "Avanzado": filtros con A/B, como el original.
class AdvancedPanel : public juce::Component {
public:
  AdvancedPanel();
  void paint(juce::Graphics &g) override;
  void resized() override;

  juce::ToggleButton click{"Quitar chasquidos y crujidos (vinilo)"};
  juce::ToggleButton noise{"Reducir ruido de fondo (casete)"};
  juce::ToggleButton eq{"Ecualizador de 31 bandas"};
  juce::ToggleButton ab{"Escuchar resultado filtrado (A/B)"};
  juce::TextButton reset{"Original"};
};

} // namespace pg
