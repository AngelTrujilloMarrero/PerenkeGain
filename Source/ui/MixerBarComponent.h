#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "app/PlaylistModel.h"
#include "ui/DeckComponent.h"
#include "ui/MasterLevelBar.h"
#include "types/Text.h"

namespace pg {

// Barra superior tipo mesa DJ: dos decks (A/B), crossfader, volumen master con
// medidor y Automix (transicion automatica encadenando la lista de reproduccion).
class MixerBarComponent : public juce::Component, private juce::Timer {
public:
  MixerBarComponent(AudioEngine &e, PlaylistModel &p);
  void paint(juce::Graphics &g) override;
  void resized() override;

  DeckComponent &deck(int i) { return i == 0 ? deckA : deckB; }

  DeckComponent deckA, deckB;

private:
  void timerCallback() override;
  void autoCheck();
  void beginTransition(int source, int target, float dur);
  void animate();
  bool loadNextInto(int target, int afterIndex);
  int playlistIndexOf(const juce::File &f) const;
  float transitionSeconds() const;

  AudioEngine &engine;
  PlaylistModel &playlist;
  juce::Slider crossfader, masterVol;
  juce::Label aL, bL, masterL;
  MasterLevelBar master;
  juce::TextButton automixB{"Automix"};
  juce::ComboBox transition;
  bool automixOn = false;
  bool animRunning = false;
  float animFrom = 0.0f, animTo = 0.0f, animPos = 0.0f, animDur = 0.0f;
  int animSource = -1;
  juce::uint32 lastMs = 0;
};

} // namespace pg
