#include "ui/MixerBarComponent.h"

namespace pg {

MixerBarComponent::MixerBarComponent(AudioEngine &e, PlaylistModel &p)
    : deckA(e, p, 0), deckB(e, p, 1), engine(e), playlist(p) {
  addAndMakeVisible(deckA);
  addAndMakeVisible(deckB);

  addAndMakeVisible(crossfader);
  crossfader.setRange(0.0, 1.0, 0.001);
  crossfader.setValue(e.crossfader(), juce::dontSendNotification);
  crossfader.setSliderStyle(juce::Slider::LinearHorizontal);
  crossfader.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  crossfader.onValueChange = [this] {
    // El usuario retoma el control manual.
    animRunning = false;
    if (automixOn) {
      automixOn = false;
      automixB.setToggleState(false, juce::dontSendNotification);
      stopTimer();
    }
    engine.setCrossfader((float)crossfader.getValue());
  };

  for (auto *l : {&aL, &bL, &masterL}) {
    addAndMakeVisible(*l);
    l->setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    l->setColour(juce::Label::textColourId, juce::Colour(0xFFF2F3F6));
    l->setInterceptsMouseClicks(false, false);
    l->setJustificationType(juce::Justification::centred);
  }
  aL.setText("A", juce::dontSendNotification);
  bL.setText("B", juce::dontSendNotification);
  masterL.setText("MASTER", juce::dontSendNotification);

  addAndMakeVisible(transition);
  transition.addItem(PG_T("Fade (8 s)"), 1);
  transition.addItem(PG_T("Fade corto (4 s)"), 2);
  transition.addItem(PG_T("Fade largo (16 s)"), 3);
  transition.addItem(PG_T("Corte"), 4);
  transition.setSelectedId(1, juce::dontSendNotification);

  addAndMakeVisible(automixB);
  automixB.setClickingTogglesState(true);
  automixB.onClick = [this] {
    automixOn = automixB.getToggleState();
    lastMs = juce::Time::getMillisecondCounter();
    if (automixOn)
      startTimerHz(20);
    else {
      animRunning = false;
      stopTimer();
    }
  };

  addAndMakeVisible(masterVol);
  masterVol.setRange(0.0, 1.0, 0.001);
  masterVol.setValue(e.masterGainValue(), juce::dontSendNotification);
  masterVol.setSliderStyle(juce::Slider::LinearHorizontal);
  masterVol.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  masterVol.onValueChange = [this] {
    engine.setMasterGain((float)masterVol.getValue());
  };

  addAndMakeVisible(master);
  master.attach(&e.bandAnalyzer());
}

float MixerBarComponent::transitionSeconds() const {
  switch (transition.getSelectedId()) {
  case 2: return 4.0f;
  case 3: return 16.0f;
  case 4: return 0.0f;
  default: return 8.0f;
  }
}

int MixerBarComponent::playlistIndexOf(const juce::File &f) const {
  for (int i = 0; i < playlist.size(); ++i)
    if (playlist.at(i).file == f)
      return i;
  return -1;
}

bool MixerBarComponent::loadNextInto(int target, int afterIndex) {
  const int next = afterIndex < 0 ? 0 : afterIndex + 1;
  if (next >= playlist.size())
    return false;
  deck(target).loadIntoDeck(playlist.at(next).file);
  return engine.hasFile(target);
}

void MixerBarComponent::timerCallback() {
  if (animRunning) {
    animate();
    return;
  }
  if (!automixOn) {
    stopTimer();
    return;
  }
  autoCheck();
}

void MixerBarComponent::autoCheck() {
  const int source = engine.isPlaying(0) ? 0 : engine.isPlaying(1) ? 1 : -1;
  if (source < 0) {
    // Nada suena: arranca un deck con pista (o carga la primera de la lista).
    int d = -1;
    for (int i = 0; i < AudioEngine::kDecks; ++i)
      if (engine.hasFile(i)) {
        d = i;
        break;
      }
    if (d < 0 && playlist.size() > 0)
      d = loadNextInto(0, -1) ? 0 : -1;
    if (d >= 0) {
      engine.setCurrentPosition(d, 0.0);
      engine.play(d);
      engine.setActiveDeck(d);
      if (deck(d).onActivated)
        deck(d).onActivated(d);
    }
    return;
  }
  const double len = engine.getLengthSec(source);
  const double pos = engine.getPositionSec(source);
  if (len <= 0.0)
    return;
  const float dur = transitionSeconds();
  const double lead = juce::jmin(juce::jmax(0.2, (double)dur), len * 0.5);
  if (len - pos > lead)
    return;

  const int target = 1 - source;
  if (!engine.hasFile(target) &&
      !loadNextInto(target, playlistIndexOf(engine.getFile(source)))) {
    automixOn = false; // no hay mas pistas
    automixB.setToggleState(false, juce::dontSendNotification);
    return;
  }
  beginTransition(source, target, dur);
}

void MixerBarComponent::beginTransition(int source, int target, float dur) {
  engine.setCurrentPosition(target, 0.0);
  engine.play(target);
  engine.setActiveDeck(target);
  if (deck(target).onActivated)
    deck(target).onActivated(target);

  animFrom = engine.crossfader();
  animTo = (target == 1) ? 1.0f : 0.0f;
  animPos = 0.0f;
  animDur = dur;
  animSource = source;
  if (dur <= 0.0f) { // corte
    engine.setCrossfader(animTo);
    crossfader.setValue(animTo, juce::dontSendNotification);
    engine.clearDeck(source);
    deck(source).resetUi();
    return;
  }
  animRunning = true;
  lastMs = juce::Time::getMillisecondCounter();
}

void MixerBarComponent::animate() {
  const juce::uint32 now = juce::Time::getMillisecondCounter();
  const float dt = (now - lastMs) / 1000.0f;
  lastMs = now;
  animPos += dt;
  const float t = juce::jlimit(0.0f, 1.0f, animPos / animDur);
  const float cross = animFrom + (animTo - animFrom) * t;
  engine.setCrossfader(cross);
  crossfader.setValue(cross, juce::dontSendNotification);
  if (t >= 1.0f) {
    animRunning = false;
    if (animSource >= 0) { // libera el deck saliente para la siguiente pista
      engine.clearDeck(animSource);
      deck(animSource).resetUi();
    }
  }
}

void MixerBarComponent::paint(juce::Graphics &g) {
  g.setColour(juce::Colour(0xFF0B0C10));
  g.fillAll();
}

void MixerBarComponent::resized() {
  auto r = getLocalBounds().reduced(4);
  const int midW = 170;
  const int sideW = (r.getWidth() - midW) / 2;
  auto left = r.removeFromLeft(sideW);
  auto right = r.removeFromRight(sideW);
  deckA.setBounds(left.reduced(2));
  deckB.setBounds(right.reduced(2));

  auto mid = r.reduced(6, 4);
  auto lab = mid.removeFromTop(14);
  aL.setBounds(lab.removeFromLeft(16));
  bL.setBounds(lab.removeFromRight(16));
  mid.removeFromTop(2);
  transition.setBounds(mid.removeFromTop(22).reduced(2, 0));
  mid.removeFromTop(2);
  crossfader.setBounds(mid.removeFromTop(22).reduced(6, 0));
  mid.removeFromTop(4);
  automixB.setBounds(mid.removeFromTop(26).reduced(24, 0));
  mid.removeFromTop(4);
  masterL.setBounds(mid.removeFromTop(12));
  auto meterRow = mid.removeFromTop(70).reduced(0, 1);
  master.setBounds(meterRow.withSizeKeepingCentre(46, 66));
  mid.removeFromTop(4);
  masterVol.setBounds(mid.removeFromTop(22).reduced(8, 0));
}

} // namespace pg
