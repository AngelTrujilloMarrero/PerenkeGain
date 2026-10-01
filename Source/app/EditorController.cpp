#include "app/EditorController.h"
#include "types/Text.h"
#include "app/DialogLauncher.h"
#include "ui/BatchNormalizeDialog.h"
#include "ui/TrackSplitterDialog.h"
#include "ui/SaveTracksDialog.h"
#include "dsp/TrackSplitterService.h"
#include "storage/AudioFileLoader.h"
#include "storage/ExportManager.h"

namespace pg {

EditorController::EditorController(AudioEngine &e, MarkerModel &m,
                                   PlaylistModel &p)
    : engine(e), markers(m), playlist(p) {}

void EditorController::setActiveFile(const juce::File &file) {
  markers.setFile(file);
  if (onFileLoaded)
    onFileLoaded(file.getFullPathName());
}

void EditorController::openSave() {
  auto *dlg = new SaveTracksDialog();
  dlg->onSave = [this](int fmt, const juce::String &album,
                       const juce::String &track) {
    saveTracks(fmt, album, track);
  };
  dialogs::show("Guardar pistas...", dlg, 440, 280);
}

void EditorController::openBatchNormalize() {
  dialogs::show(PG_T("Normalizar audios por lotes"),
                new BatchNormalizeDialog(), 540, 500);
}

void EditorController::openSplitter() {
  auto *dlg = new TrackSplitterDialog();
  dlg->onAnalyze = [this](const SilenceParams &p) { splitBySilence(p); };
  dialogs::show("Dividir en pistas", dlg, 400, 380);
}

// Detecta silencios y los fusiona con las marcas manuales existentes para
// producir segmentos (pistas) no solapados.
void EditorController::splitBySilence(const SilenceParams &params) {
  const juce::File file = markers.file();
  if (file == juce::File{})
    return;
  juce::AudioBuffer<float> buf;
  double sr = 44100.0;
  if (!AudioFileLoader::load(file, buf, sr))
    return;

  TrackSplitterService svc;
  auto detected = svc.analyze(buf, sr, params);
  const double total = (double)buf.getNumSamples() / sr;

  std::vector<double> cuts;
  for (const auto &m : markers.markers()) { // marcas manuales respetadas
    cuts.push_back(m.startSec);
    if (m.endSec > m.startSec)
      cuts.push_back(m.endSec);
  }
  for (size_t i = 1; i < detected.size(); ++i)
    cuts.push_back(detected[i].startSec);

  markers.setMarkers(MarkerModel::segmentsFromCuts(cuts, total));
}

void EditorController::saveTracks(int formatId, const juce::String &,
                                  const juce::String &) {
  const juce::File file = markers.file();
  if (file == juce::File{})
    return;
  dirChooser = std::make_unique<juce::FileChooser>(
      "Carpeta de salida", file.getParentDirectory(), "*");
  dirChooser->launchAsync(
      juce::FileBrowserComponent::openMode |
          juce::FileBrowserComponent::canSelectDirectories,
      [this, formatId, file](const juce::FileChooser &fc) {
        auto dir = fc.getResult();
        if (dir == juce::File{})
          return;
        juce::AudioBuffer<float> buf;
        double sr = 44100.0;
        if (!AudioFileLoader::load(file, buf, sr))
          return;
        const double total = (double)buf.getNumSamples() / sr;
        std::vector<double> cuts;
        for (const auto &m : markers.markers()) {
          cuts.push_back(m.startSec);
          if (m.endSec > m.startSec)
            cuts.push_back(m.endSec);
        }
        auto regs = MarkerModel::segmentsFromCuts(cuts, total);
        ExportManager ex;
        ex.exportTracks(buf, sr, regs, dir, formatId);
      });
}

void EditorController::quitEditor() {
  juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

} // namespace pg
