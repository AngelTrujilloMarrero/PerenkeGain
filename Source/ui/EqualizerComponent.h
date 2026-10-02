#pragma once
#include <JuceHeader.h>
#include "types/EqTypes.h"
#include "ui/EqPresetBar.h"
#include "ui/MixerRowComponent.h"

namespace pg {

// EQ gráfica 31 bandas simulando una mesa de sonido digital moderna:
// tiras de canal (medidor + fader + MUTE), lector LCD y master abajo.
// La intensidad por banda queda fuera de la UI (fija al 100 %).
class EqualizerComponent : public juce::Component {
public:
  EqualizerComponent();
  void paint(juce::Graphics &g) override;
  void resized() override;
  Eq31State getState() const;
  void setState(const Eq31State &s);

  // Carga un preset preconfigurado (indice en kEqPresets) y lo aplica.
  void applyPreset(int index);

  // Restaura una sesión guardada (preset o personalizado).
  void restoreSaved(const Eq31State &s, int preset, bool custom);
  int currentPreset() const;
  bool isCustom() const;

  EqPresetBar presetBar;
  MixerRowComponent mixer; // el root lo enlaza con el analizador

  // Se dispara al cambiar cualquier banda (gain/mute) o el master.
  std::function<void()> onStateChanged;

private:
  juce::Slider master;
  juce::Label masterTitle, freqLabelsTitle, readout;
  int freqLabelsTop = 0;
  int colW() const;
};

} // namespace pg
