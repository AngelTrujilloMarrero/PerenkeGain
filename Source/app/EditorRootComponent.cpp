#include "app/EditorRootComponent.h"
#include "app/DialogLauncher.h"
#include "BinaryData.h"
#include "types/Text.h"
#include "ui/AboutDialog.h"
#include "ui/UpdateDialog.h"

namespace pg {

EditorRootComponent::EditorRootComponent()
    : playlistComp(engine, playlist), mixer(engine, playlist),
      controller(engine, markerModel, playlist),
      ticker(engine, cyan, progressPct) {
  updateBanner.onShowDetails = [this] { openUpdateDialog(); };
  updateBanner.onDismiss = [this] {
    updateBanner.setVisible(false);
    layoutRows();
  };
  addChildComponent(updateBanner);

  addAndMakeVisible(filePath);
  filePath.setColour(juce::Label::backgroundColourId, juce::Colours::white);
  filePath.setColour(juce::Label::textColourId, juce::Colours::black);
  filePath.setFont(juce::Font(juce::FontOptions(12.0f)));
  filePath.setText(PG_T("Sin archivo"), juce::dontSendNotification);

  addAndMakeVisible(mixer);
  addAndMakeVisible(cyan);
  addAndMakeVisible(progressPct);
  progressPct.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
  progressPct.setColour(juce::Label::backgroundColourId,
                        juce::Colour(0xFF14151A));
  progressPct.setColour(juce::Label::textColourId, juce::Colour(0xFF7CFF3C));
  progressPct.setJustificationType(juce::Justification::centred);
  progressPct.setInterceptsMouseClicks(false, false);
  addAndMakeVisible(search);
  addAndMakeVisible(playlistComp);
  addAndMakeVisible(dock);
  for (auto *b : {&addB, &waveB, &batchB, &closeB})
    addAndMakeVisible(*b);

  addAndMakeVisible(aboutB);
  auto badge = juce::ImageCache::getFromMemory(BinaryData::logobadge_png,
                                               BinaryData::logobadge_pngSize);
  aboutB.setImages(false, true, true, badge, 1.0f, {}, badge, 0.85f, {},
                   badge, 0.7f, {});
  aboutB.setTooltip(PG_T("Acerca de PerenkeGain"));
  aboutB.onClick = [this] { openAbout(); };

  dock.eq.mixer.attach(&engine.bandAnalyzer());
  dock.leveler.setEngine(&engine);
  // Restaura la última ecualización guardada (si hay) antes de arrancar.
  if (EqSavedState saved = eqStore.load(); saved.hasData)
    dock.eq.restoreSaved(saved.state, saved.preset, saved.custom);
  dock.eq.onStateChanged = [this] {
    engine.setEqState(dock.eq.getState());
    eqStore.save(dock.eq.getState(), dock.eq.currentPreset(),
                 dock.eq.isCustom());
  };
  engine.setEqState(dock.eq.getState());

  controller.onFileLoaded = [this](const juce::String &path) {
    filePath.setText(path, juce::dontSendNotification);
  };
  wire();

  // Arranque con archivo por línea de comandos: PerenkeGain archivo.wav
  if (auto *app = juce::JUCEApplication::getInstance()) {
    const auto &params = app->getCommandLineParameterArray();
    if (!params.isEmpty()) {
      auto f = juce::File(params[0]);
      if (f.existsAsFile())
        enqueue(f);
    }
  }

  checkForUpdates();
}

void EditorRootComponent::wire() {
  mixer.deck(0).onActivated = [this](int d) { activateDeck(d); };
  mixer.deck(1).onActivated = [this](int d) { activateDeck(d); };
  search.onDownloaded = [this](const juce::File &f) { enqueue(f); };
  playlistComp.onSendToDeck = [this](int deck, const juce::File &f) {
    mixer.deck(deck).loadIntoDeck(f);
  };

  addB.onClick = [this] { addLocalFiles(); };
  waveB.onClick = [this] { showWaveformWindow(); };
  batchB.onClick = [this] { controller.openBatchNormalize(); };
  closeB.onClick = [this] { controller.quitEditor(); };

  cyan.onSeekFraction = [this](double f) {
    const int d = engine.activeDeck();
    if (engine.hasFile(d))
      engine.setCurrentPosition(d, f * engine.getLengthSec(d));
  };
}

void EditorRootComponent::activateDeck(int deck) {
  controller.setActiveFile(engine.getFile(deck));
}

// La ventana de onda se crea solo al pedirla: en Linux, crearla al arrancar
// hacia que apareciera sola y no se pudiera cerrar.
void EditorRootComponent::showWaveformWindow() {
  if (waveformWindow == nullptr) {
    waveformWindow = std::make_unique<WaveformWindow>(engine, markerModel);
    waveformWindow->editor().onRequestSplit = [this] {
      controller.openSplitter();
    };
    waveformWindow->editor().onRequestSave = [this] { controller.openSave(); };
  }
  waveformWindow->setVisible(true);
  waveformWindow->toFront(true);
}

int EditorRootComponent::firstEmptyDeck() const {
  for (int d = 0; d < AudioEngine::kDecks; ++d)
    if (!engine.hasFile(d))
      return d;
  return -1;
}

void EditorRootComponent::enqueue(const juce::File &file) {
  if (!file.existsAsFile())
    return;
  playlist.add(file);
  // Solo carga si hay un deck libre; asi no se detiene lo que suena.
  const int d = firstEmptyDeck();
  if (d >= 0)
    mixer.deck(d).loadIntoDeck(file);
}

void EditorRootComponent::addLocalFiles() {
  picker.chooseFiles(PG_T("Añadir canciones"),
                     "*.wav;*.mp3;*.flac;*.ogg;*.aiff;*.aif", juce::File{},
                     [this](const juce::Array<juce::File> &files) {
                       // Llena los decks libres en orden (A, B, ...).
                       for (const auto &f : files)
                         enqueue(f);
                     });
}

void EditorRootComponent::checkForUpdates() {
  auto safe = juce::Component::SafePointer<EditorRootComponent>(this);
  updateChecker.checkAsync([safe](updater::UpdateInfo info) {
    if (safe != nullptr && info.available)
      safe->showUpdateAvailable(info);
  });
}

void EditorRootComponent::showUpdateAvailable(
    const updater::UpdateInfo &info) {
  pendingUpdate = info;
  updateBanner.setVersion(info.latestVersion, info.currentVersion);
  updateBanner.setVisible(true);
  layoutRows();
}

void EditorRootComponent::openAbout() {
  dialogs::show(PG_T("Acerca de PerenkeGain"), new AboutDialog(), 560, 660);
}

void EditorRootComponent::openUpdateDialog() {
  auto *dlg = new UpdateDialog(pendingUpdate);
  dialogs::show(PG_T("Actualizaci\u00f3n de PerenkeGain"), dlg, 500, 440);
}

void EditorRootComponent::layoutRows() {
  auto r = getLocalBounds().reduced(3);
  if (updateBanner.isVisible())
    updateBanner.setBounds(r.removeFromTop(36).reduced(0, 2));
  mixer.setBounds(r.removeFromTop(258));

  auto toolbar = r.removeFromTop(30);
  auto right = toolbar.removeFromRight(540);
  closeB.setBounds(right.removeFromRight(90).reduced(3, 2));
  batchB.setBounds(right.removeFromRight(160).reduced(3, 2));
  waveB.setBounds(right.removeFromRight(170).reduced(3, 2));
  addB.setBounds(right.removeFromRight(110).reduced(3, 2));
  aboutB.setBounds(toolbar.removeFromLeft(28).reduced(1));
  filePath.setBounds(toolbar.reduced(2));

  auto cyanRow = r.removeFromTop(26).reduced(0, 3);
  progressPct.setBounds(cyanRow.removeFromRight(52));
  cyan.setBounds(cyanRow);
  dock.setBounds(r.removeFromBottom(dock.preferredHeight()));
  // Centro: buscador (solo 3 resultados) arriba y lista de reproduccion abajo.
  search.setBounds(r.removeFromTop(116));
  playlistComp.setBounds(r);
}

void EditorRootComponent::resized() { layoutRows(); }

} // namespace pg
