#include "app/EditorController.h"
#include "types/Text.h"
#include "app/DialogLauncher.h"
#include "ui/AdvancedPanel.h"
#include "ui/TrackSplitterDialog.h"
#include "ui/SaveTracksDialog.h"
#include "dsp/TrackSplitterService.h"
#include "storage/AudioFileLoader.h"
#include "storage/ExportManager.h"
#include <algorithm>

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
  currentFile = file;
  markers.clear();
  wave.setMarkers(markers);
  info.setFile({}, engine.getLengthSec());
  info.setQuality(engine.getSourceInfo());
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

void EditorController::openAdvanced() {
  dialogs::show(PG_T("Avanzado - Filtros de restauración"), new AdvancedPanel(),
                460, 320);
}

void EditorController::openSplitter() {
  auto *dlg = new TrackSplitterDialog();
  dlg->onAnalyze = [this](const SilenceParams &p) { splitBySilence(p); };
  dialogs::show("Dividir en pistas", dlg, 400, 380);
}

// Detecta silencios y los fusiona con las marcas manuales existentes para
// producir segmentos (pistas) no solapados.
void EditorController::splitBySilence(const SilenceParams &params) {
  if (currentFile == juce::File{})
    return;
  juce::AudioBuffer<float> buf;
  double sr = 44100.0;
  if (!AudioFileLoader::load(currentFile, buf, sr))
    return;

  TrackSplitterService svc;
  auto detected = svc.analyze(buf, sr, params);
  const double total = (double)buf.getNumSamples() / sr;

  std::vector<double> cuts;
  for (const auto &m : markers) { // marcas manuales respetadas
    cuts.push_back(m.startSec);
    if (m.endSec > m.startSec)
      cuts.push_back(m.endSec);
  }
  for (size_t i = 1; i < detected.size(); ++i)
    cuts.push_back(detected[i].startSec);

  markers = segmentsFromCuts(cuts, total);
  wave.setMarkers(markers);
}

std::vector<TrackRegion>
EditorController::segmentsFromCuts(std::vector<double> cuts,
                                   double total) const {
  cuts.push_back(0.0);
  cuts.push_back(total);
  std::sort(cuts.begin(), cuts.end());
  std::vector<double> uniq;
  for (double c : cuts) {
    c = juce::jlimit(0.0, total, c);
    if (uniq.empty() || c - uniq.back() > 0.05)
      uniq.push_back(c);
  }
  std::vector<TrackRegion> segs;
  for (size_t i = 0; i + 1 < uniq.size(); ++i)
    segs.push_back({uniq[i], uniq[i + 1], (int)i});
  return segs;
}

void EditorController::saveTracks(int formatId, const juce::String &,
                                  const juce::String &) {
  if (currentFile == juce::File{})
    return;
  dirChooser = std::make_unique<juce::FileChooser>(
      "Carpeta de salida", currentFile.getParentDirectory(), "*");
  dirChooser->launchAsync(
      juce::FileBrowserComponent::openMode |
          juce::FileBrowserComponent::canSelectDirectories,
      [this, formatId](const juce::FileChooser &fc) {
        auto dir = fc.getResult();
        if (dir == juce::File{})
          return;
        juce::AudioBuffer<float> buf;
        double sr = 44100.0;
        if (!AudioFileLoader::load(currentFile, buf, sr))
          return;
        const double total = (double)buf.getNumSamples() / sr;
        std::vector<double> cuts;
        for (const auto &m : markers) {
          cuts.push_back(m.startSec);
          if (m.endSec > m.startSec)
            cuts.push_back(m.endSec);
        }
        auto regs = segmentsFromCuts(cuts, total);
        ExportManager ex;
        ex.exportTracks(buf, sr, regs, dir, formatId);
      });
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
