#pragma once
#include <JuceHeader.h>
#include "types/Text.h"
#include "audio/AudioEngine.h"
#include "audio/WaveformComponent.h"
#include "audio/TransportComponent.h"
#include "app/EditorController.h"
#include "app/PlaybackTicker.h"
#include "ui/FileInfoBar.h"
#include "ui/CyanProgressBar.h"
#include "ui/HelpButton.h"
#include "ui/BottomDockComponent.h"

namespace pg {

// Layout fiel al Editor de Sonido PolderbitS original (capturas Malavida).
class EditorRootComponent : public juce::Component {
public:
  EditorRootComponent();
  void resized() override;

private:
  AudioEngine engine;
  WaveformComponent wave;
  FileInfoBar info;
  CyanProgressBar cyan;
  TransportComponent transport;
  HelpButton helpBtn;
  BottomDockComponent dock;
  EditorController controller;
  PlaybackTicker ticker;

  juce::Label filePath;
  juce::TextButton openB{"Abrir..."}, saveB{"Guardar como..."},
      splitB{"Dividir..."}, eqB{"Ecualizador..."}, advB{"Avanzado..."},
      closeB{"Cerrar"}, skipB{PG_T("→5")}, collapseB{PG_T("▲")};
  juce::ToggleButton cutBox{"Iniciar Corte"},
      fadeIn{"Fade In / Punto de Inicio"}, fadeOut{"Fade Out / Punto del Final"};
  juce::Label escalaTitle;
  juce::Slider escala;

  void wireButtons();
  void layoutRows();
};

} // namespace pg
