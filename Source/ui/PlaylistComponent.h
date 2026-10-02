#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "app/PlaylistModel.h"
#include "types/Text.h"

namespace pg {

// Lista de reproduccion central: muestra a que deck va cada pista y permite
// reordenar (arrastrando o subiendo/bajando), quitar y enviar a un deck.
// El arrastre (a otro orden o a un deck) lo gestiona el DragAndDropContainer
// del componente raíz; doble clic envía al primer deck libre.
class PlaylistComponent : public juce::Component,
                          public juce::DragAndDropTarget,
                          private juce::ListBoxModel,
                          private juce::ChangeListener,
                          private juce::Timer {
public:
  PlaylistComponent(AudioEngine &e, PlaylistModel &p);
  ~PlaylistComponent() override;
  void paint(juce::Graphics &g) override;
  void resized() override;

  std::function<void(int deck, const juce::File &)> onSendToDeck;

private:
  void changeListenerCallback(juce::ChangeBroadcaster *) override;
  void timerCallback() override;
  int getNumRows() override;
  void paintListBoxItem(int row, juce::Graphics &g, int width, int height,
                        bool rowIsSelected) override;
  juce::var getDragSourceDescription(const juce::SparseSet<int> &rows) override;
  void listBoxItemDoubleClicked(int row, const juce::MouseEvent &) override;
  bool isInterestedInDragSource(const SourceDetails &) override;
  void itemDropped(const SourceDetails &details) override;

  void moveSelected(int delta);
  void removeSelected();
  void sendSelected(int deck);
  void sendToFreeDeck(int row);
  int selectedEntry() const;
  int deckOf(const juce::File &f) const;

  AudioEngine &engine;
  PlaylistModel &playlist;
  juce::ListBox list;
  juce::TextButton upB{"Subir"}, downB{"Bajar"}, removeB{"Quitar"},
      toAB{PG_T("\u2192 A")}, toBB{PG_T("\u2192 B")};
};

} // namespace pg
