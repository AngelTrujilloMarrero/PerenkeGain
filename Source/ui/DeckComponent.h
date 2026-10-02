#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "app/PlaylistModel.h"
#include "ui/TransportButton.h"
#include "types/Text.h"

namespace pg {

// Un deck de la mesa: selector de pista de la lista, titulo y transporte.
// Acepta pista soltando encima una fila de la lista o un archivo de audio
// del sistema, además del desplegable.
class DeckComponent : public juce::Component,
                      public juce::DragAndDropTarget,
                      public juce::FileDragAndDropTarget,
                      private juce::ChangeListener,
                      private juce::Timer {
public:
  DeckComponent(AudioEngine &e, PlaylistModel &p, int deckIndex);
  ~DeckComponent() override;
  void paint(juce::Graphics &g) override;
  void resized() override;

  // Carga una pista en este deck, la marca como activa y avisa al root.
  void loadIntoDeck(const juce::File &f);
  // Refresca la UI tras vaciar el deck desde fuera (automix).
  void resetUi();
  int deckIndex() const { return index; }
  std::function<void(int)> onActivated;

  bool isInterestedInDragSource(const SourceDetails &details) override;
  void itemDropped(const SourceDetails &details) override;
  void itemDragEnter(const SourceDetails &) override;
  void itemDragExit(const SourceDetails &) override;
  bool isInterestedInFileDrag(const juce::StringArray &files) override;
  void filesDropped(const juce::StringArray &files, int x, int y) override;
  void fileDragEnter(const juce::StringArray &, int x, int y) override;
  void fileDragExit(const juce::StringArray &) override;

private:
  void changeListenerCallback(juce::ChangeBroadcaster *) override;
  void timerCallback() override;
  void rebuild();
  void syncSelection();
  void updateTitle();
  void chooseFromBox();

  AudioEngine &engine;
  PlaylistModel &playlist;
  int index;
  juce::ComboBox trackBox;
  juce::Label title, timeL;
  TransportButton playB{TransportButton::Icon::Play};
  TransportButton stopB{TransportButton::Icon::Stop};
  juce::File loadedFile;
  bool dragOver = false;
};

} // namespace pg
