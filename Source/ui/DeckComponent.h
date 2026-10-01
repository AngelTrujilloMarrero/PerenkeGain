#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "app/PlaylistModel.h"
#include "ui/TransportButton.h"
#include "types/Text.h"

namespace pg {

// Un deck de la mesa: selector de pista de la lista, titulo y transporte.
class DeckComponent : public juce::Component,
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
  int lastCount = -1;
};

} // namespace pg
