#pragma once
#include <JuceHeader.h>
#include <functional>

namespace pg {

// Fila "Preset: [desplegable]" del ecualizador. Lista los presets
// compilados y muestra "Personalizado" cuando el usuario mueve un fader
// a mano despues de cargar uno.
class EqPresetBar : public juce::Component {
public:
  EqPresetBar();
  void resized() override;

  // Indice dentro de kEqPresets elegido por el usuario.
  std::function<void(int)> onPresetChosen;

  // Marca el preset activo sin disparar onPresetChosen.
  void select(int index);
  // Muestra "Personalizado" sin disparar onPresetChosen.
  void showCustom();

private:
  juce::Label title;
  juce::ComboBox box;
  int lastPreset = 0;
};

} // namespace pg
