#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "app/MarkerModel.h"
#include "app/PlaylistModel.h"
#include "types/AudioTypes.h"

namespace pg {

// Acciones del editor: cargar, guardar, dividir y normalizar.
class EditorController {
public:
  EditorController(AudioEngine &e, MarkerModel &m, PlaylistModel &p);
  void setActiveFile(const juce::File &file); // marcadores + info de la activa
  void openSave();
  void openBatchNormalize();
  void openSplitter();
  void quitEditor();

  // El root lo usa para pintar la ruta.
  std::function<void(const juce::String &)> onFileLoaded;

private:
  void splitBySilence(const SilenceParams &params);
  void saveTracks(int formatId, const juce::String &album,
                  const juce::String &track);

  AudioEngine &engine;
  MarkerModel &markers;
  PlaylistModel &playlist;
  std::unique_ptr<juce::FileChooser> dirChooser;
};

} // namespace pg
