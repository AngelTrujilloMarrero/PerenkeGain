#pragma once
#include <JuceHeader.h>
#include "types/Text.h"
#include "audio/AudioEngine.h"
#include "app/EditorController.h"
#include "app/MarkerModel.h"
#include "app/PlaylistModel.h"
#include "app/PlaybackTicker.h"
#include "app/WaveformWindow.h"
#include "ui/CyanProgressBar.h"
#include "ui/FilePicker.h"
#include "ui/MixerBarComponent.h"
#include "ui/PlaylistComponent.h"
#include "ui/YouTubeSearchPanel.h"
#include "ui/BottomDockComponent.h"
#include "ui/UpdateBannerComponent.h"
#include "storage/EqSettingsStore.h"
#include "updater/UpdateChecker.h"
#include "updater/UpdateInfo.h"

namespace pg {

// Ventana principal: mesa DJ (dos decks + crossfader) arriba, buscador de
// YouTube, informacion y la bandeja con ecualizador y nivelador. La edicion de
// onda vive en una ventana aparte (WaveformWindow). Es el DragAndDropContainer
// común: permite arrastrar filas de la lista a los decks y reordenarlas.
class EditorRootComponent : public juce::Component,
                            public juce::DragAndDropContainer {
public:
  EditorRootComponent();
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  AudioEngine engine;
  PlaylistModel playlist;
  MarkerModel markerModel;
  CyanProgressBar cyan;
  juce::Label progressPct;
  BottomDockComponent dock;
  YouTubeSearchPanel search;
  PlaylistComponent playlistComp;
  MixerBarComponent mixer;
  std::unique_ptr<WaveformWindow> waveformWindow; // se crea al pedirla
  EditorController controller;
  PlaybackTicker ticker;
  UpdateBannerComponent updateBanner;
  updater::UpdateChecker updateChecker;
  updater::UpdateInfo pendingUpdate;
  EqSettingsStore eqStore;

  juce::Label filePath;
  juce::ImageButton aboutB{"logo"};
  juce::TextButton addB{PG_T("Añadir...")}, waveB{"Editor de onda..."},
      batchB{"Normalizar lote..."}, closeB{"Cerrar"};
  juce::ToggleButton eqB{PG_T("EQ")};
  bool dockVisible = true;
  bool dockToggled = false;
  FilePicker picker;

  void wire();
  void layoutRows();
  void setDockVisible(bool v);
  void activateDeck(int deck);
  void showWaveformWindow();
  int firstEmptyDeck() const; // deck libre; -1 si ambos tienen pista
  void enqueue(const juce::File &file);
  void addLocalFiles();
  void openAbout();
  void checkForUpdates();
  void showUpdateAvailable(const updater::UpdateInfo &info);
  void openUpdateDialog();
};

} // namespace pg
