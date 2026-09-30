#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "audio/WaveformComponent.h"
#include "ui/FileInfoBar.h"
#include "types/AudioTypes.h"

namespace pg {

// Acciones del editor: abrir/guardar, diálogos y marcadores de corte.
class EditorController {
public:
  EditorController(AudioEngine &e, WaveformComponent &w, FileInfoBar &f);
  void openAudio();
  void openFilePath(const juce::File &file); // carga directa (CLI, tests)
  void openSave();
  void openAdvanced();
  void openSplitter();
  void addMarkerAtPlayhead();
  void quitEditor();

  // El root lo usa para pintar la ruta en el campo superior.
  std::function<void(const juce::String &)> onFileLoaded;

private:
  AudioEngine &engine;
  WaveformComponent &wave;
  FileInfoBar &info;
  std::vector<TrackRegion> markers;
  std::unique_ptr<juce::FileChooser> chooser;
};

} // namespace pg
