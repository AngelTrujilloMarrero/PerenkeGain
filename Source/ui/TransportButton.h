#pragma once
#include <JuceHeader.h>

namespace pg {

// Boton de transporte moderno: circulo plano con icono (play/pausa/stop),
// pensado para los paneles oscuros de los decks.
class TransportButton : public juce::Button {
public:
  enum class Icon { Play, Pause, Stop };

  explicit TransportButton(Icon i);
  void setIcon(Icon i);

  void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted,
                   bool shouldDrawButtonAsDown) override;

private:
  Icon icon;
};

} // namespace pg
