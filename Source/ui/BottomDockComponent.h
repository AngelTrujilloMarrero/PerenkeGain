#pragma once
#include <JuceHeader.h>
#include "ui/HelpInfoPanel.h"
#include "ui/EqualizerComponent.h"

namespace pg {

// Bandeja inferior del editor: muestra el panel de ayuda clásico o el
// ecualizador integrado en la parte inferior (sin ventana emergente).
// La altura preferida cambia según el modo activo.
class BottomDockComponent : public juce::Component {
public:
  BottomDockComponent();
  void paint(juce::Graphics &g) override;
  void resized() override;

  void showHelp();
  void showEqualizer();
  bool isHelpMode() const { return helpMode; }
  int preferredHeight() const { return helpMode ? 128 : 360; }

  // Se dispara al pulsar "Continuar" del panel de ayuda.
  std::function<void()> onHelpContinue;

  HelpInfoPanel help;
  EqualizerComponent eq;

private:
  juce::TextButton backBtn{"Volver"};
  bool helpMode = true;
};

} // namespace pg
