#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "audio/WaveformComponent.h"
#include "app/MarkerModel.h"
#include "ui/MasterLevelBar.h"
#include "types/Text.h"

namespace pg {

// Editor de onda (contenido de la ventana emergente): forma de onda con
// marcadores, medidor master, corte/fade/escala y boton de dividir.
class WaveformEditorComponent : public juce::Component,
                                private juce::Timer,
                                private juce::ChangeListener {
public:
  WaveformEditorComponent(AudioEngine &e, MarkerModel &m);
  ~WaveformEditorComponent() override;
  void paint(juce::Graphics &g) override;
  void resized() override;

  // El root los enlaza con el dialogo de division y con guardar pistas.
  std::function<void()> onRequestSplit;
  std::function<void()> onRequestSave;

private:
  void timerCallback() override;
  void changeListenerCallback(juce::ChangeBroadcaster *) override;

  AudioEngine &engine;
  MarkerModel &model;
  WaveformComponent wave;
  MasterLevelBar meter;
  juce::ToggleButton cutBox{"Iniciar Corte"},
      fadeIn{"Fade In / Punto de Inicio"},
      fadeOut{"Fade Out / Punto del Final"};
  juce::Label escalaTitle;
  juce::Slider escala;
  juce::TextButton splitB{"Dividir..."}, saveB{"Guardar como..."};
  juce::File lastFile;
};

} // namespace pg
