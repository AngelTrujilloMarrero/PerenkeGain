#include "app/EditorController.h"
#include "types/Text.h"
#include "app/DialogLauncher.h"
#include "ui/AdvancedPanel.h"
#include "ui/TrackSplitterDialog.h"
#include "ui/SaveTracksDialog.h"

namespace pg {

EditorController::EditorController(AudioEngine &e, WaveformComponent &w,
                                   FileInfoBar &f)
    : engine(e), wave(w), info(f) {}

void EditorController::openAudio() {
  chooser = std::make_unique<juce::FileChooser>(
      "Abrir archivo de sonido", juce::File{},
      "*.wav;*.mp3;*.flac;*.ogg;*.aac");
  chooser->launchAsync(
      juce::FileBrowserComponent::openMode |
          juce::FileBrowserComponent::canSelectFiles,
      [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();
        if (file == juce::File{})
          return;
        openFilePath(file);
      });
}

void EditorController::openFilePath(const juce::File &file) {
  engine.loadFile(file);
  wave.openFile(file);
  markers.clear();
  wave.setMarkers(markers);
  info.setFile({}, engine.getLengthSec());
  info.setQuality(engine.getSourceInfo());
  if (onFileLoaded)
    onFileLoaded(file.getFullPathName());
}

void EditorController::openSave() {
  dialogs::show("Guardar pistas...", new SaveTracksDialog(), 440, 260);
}

void EditorController::openAdvanced() {
  dialogs::show(PG_T("Avanzado - Filtros de restauración"), new AdvancedPanel(),
                460, 320);
}

void EditorController::openSplitter() {
  dialogs::show("Dividir en pistas", new TrackSplitterDialog(), 400, 340);
}

// Con selección activa en la onda: el corte usa los extremos del tramo
// seleccionado; sin selección, marca un punto en el cabezal.
void EditorController::addMarkerAtPlayhead() {
  double a = engine.getPositionSec(), b = a;
  if (wave.hasSelection()) {
    auto s = wave.getSelection();
    a = s.first;
    b = s.second;
  }
  TrackRegion m{a, b, (int)markers.size()};
  markers.push_back(m);
  wave.setMarkers(markers);
}

void EditorController::quitEditor() {
  juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

} // namespace pg
