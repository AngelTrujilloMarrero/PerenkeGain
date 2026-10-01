#pragma once
#include <JuceHeader.h>
#include "ui/WaveformEditorComponent.h"

namespace pg {

// Ventana independiente y redimensionable con el editor de onda. Cerrarla
// solo la oculta (se puede reabrir desde la ventana principal).
class WaveformWindow : public juce::DocumentWindow {
public:
  WaveformWindow(AudioEngine &e, MarkerModel &m);
  void closeButtonPressed() override;
  WaveformEditorComponent &editor() { return *content; }

private:
  WaveformEditorComponent *content = nullptr;
};

} // namespace pg
