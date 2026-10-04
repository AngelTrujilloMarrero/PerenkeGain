#include "ui/DeckComponent.h"

namespace pg {

DeckComponent::DeckComponent(AudioEngine &e, PlaylistModel &p, int deckIndex)
    : engine(e), playlist(p), index(deckIndex) {
  setOpaque(true);
  addAndMakeVisible(trackBox);
  trackBox.setTextWhenNothingSelected(PG_T("Elegir pista..."));
  trackBox.onChange = [this] { chooseFromBox(); };

  addAndMakeVisible(title);
  title.setFont(juce::Font(juce::FontOptions(20.0f, juce::Font::bold)));
  title.setColour(juce::Label::textColourId, juce::Colour(0xFF7CFF3C));
  title.setText(PG_T("Sin pista"), juce::dontSendNotification);
  title.setJustificationType(juce::Justification::centred);
  title.setInterceptsMouseClicks(false, false);

  addAndMakeVisible(timeL);
  timeL.setFont(juce::Font(juce::FontOptions(12.0f)));
  timeL.setColour(juce::Label::textColourId, juce::Colour(0xFF9AA0AC));
  timeL.setJustificationType(juce::Justification::centred);
  timeL.setInterceptsMouseClicks(false, false);
  timeL.setText("--:-- / --:--", juce::dontSendNotification);

  addAndMakeVisible(playB);
  addAndMakeVisible(stopB);
  playB.onClick = [this] {
    engine.setActiveDeck(index);
    engine.togglePlayPause(index);
    playB.setIcon(engine.isPlaying(index) ? TransportButton::Icon::Pause
                                          : TransportButton::Icon::Play);
    if (onActivated)
      onActivated(index);
  };
  stopB.onClick = [this] {
    engine.stop(index);
    playB.setIcon(TransportButton::Icon::Play);
  };

  setupButtons();

  playlist.addChangeListener(this);
  rebuild();
  startTimerHz(5);
}

DeckComponent::~DeckComponent() {
  stopTimer();
  playlist.removeChangeListener(this);
}

void DeckComponent::timerCallback() {
  playB.setIcon(engine.isPlaying(index) ? TransportButton::Icon::Pause
                                        : TransportButton::Icon::Play);
  const double len = engine.getLengthSec(index);
  const double pos = engine.getPositionSec(index);
  // Al terminar la pista, libera el deck y la quita de la lista (si el
  // otro deck no esta usando el mismo fichero).
  if (engine.hasFile(index) && !engine.isPlaying(index) && len > 0.0 &&
      pos >= len - 0.05) {
    const juce::File finished = engine.getFile(index);
    releaseDeck();
    if (finished != juce::File{} && engine.getFile(1 - index) != finished) {
      const int row = playlist.indexOf(finished);
      if (row >= 0)
        playlist.remove(row);
    }
    if (onActivated)
      onActivated(engine.activeDeck());
    return;
  }
  auto fmt = [](double s) {
    if (s < 0.0)
      s = 0.0;
    const int total = (int)s;
    return juce::String(total / 60) + ":" +
           juce::String(total % 60).paddedLeft('0', 2);
  };
  timeL.setText(fmt(pos) + " / " +
                    (len > 0.0 ? fmt(len) : juce::String("--:--")),
                juce::dontSendNotification);
}

void DeckComponent::changeListenerCallback(juce::ChangeBroadcaster *) {
  rebuild();
}

void DeckComponent::rebuild() {
  // Reconstruye siempre: la lista puede reordenarse sin cambiar de tamaño
  // y el desplegable quedaba con títulos/orden viejos (elegir pista fallaba).
  trackBox.clear(juce::dontSendNotification);
  for (int i = 0; i < playlist.size(); ++i)
    trackBox.addItem(playlist.at(i).title, i + 1);
  syncSelection();
}

void DeckComponent::syncSelection() {
  int id = 0;
  if (loadedFile != juce::File{}) {
    for (int i = 0; i < playlist.size(); ++i)
      if (playlist.at(i).file == loadedFile) {
        id = i + 1;
        break;
      }
  }
  trackBox.setSelectedId(id, juce::dontSendNotification);
}

void DeckComponent::chooseFromBox() {
  const int idx = trackBox.getSelectedItemIndex();
  if (idx < 0 || idx >= playlist.size())
    return;
  loadIntoDeck(playlist.at(idx).file);
}

void DeckComponent::loadIntoDeck(const juce::File &f) {
  loadedFile = f;
  // No se toca el deck activo: la barra y la onda siguen al que suena y
  // solo cambian cuando este deck empiece a reproducir.
  engine.loadFile(index, f);
  syncSelection();
  updateTitle();
  if (onActivated)
    onActivated(index);
}

void DeckComponent::resetUi() {
  loadedFile = juce::File{};
  updateTitle();
  syncSelection();
  timeL.setText("--:-- / --:--", juce::dontSendNotification);
}

void DeckComponent::updateTitle() {
  const juce::File f = engine.getFile(index);
  title.setText(f == juce::File{} ? PG_T("Sin pista")
                                  : f.getFileNameWithoutExtension(),
                juce::dontSendNotification);
  clearB.setEnabled(f != juce::File{});
}

void DeckComponent::paint(juce::Graphics &g) {
  g.setColour(index == 0 ? juce::Colour(0xFF17202E)
                         : juce::Colour(0xFF2A1D2E));
  g.fillAll();
  g.setColour(dragOver ? juce::Colour(0xFF00E0E0)
                       : juce::Colour(0xFF3A4152));
  g.drawRect(getLocalBounds(), dragOver ? 2 : 1);
  g.setColour(juce::Colour(0xFFB9C2D0));
  g.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
  g.drawText(index == 0 ? "A" : "B", getLocalBounds().reduced(6, 2),
             juce::Justification::topLeft);
}

void DeckComponent::resized() {
  auto r = getLocalBounds().reduced(8);
  // Cabecera: letras A/B (pintadas) a la izquierda, botones a la derecha.
  auto header = r.removeFromTop(20);
  loadB.setBounds(header.removeFromRight(74).reduced(1, 1));
  clearB.setBounds(header.removeFromRight(24).reduced(1, 1));
  // Transporte centrado.
  auto transport = r.removeFromBottom(40);
  const int bw = 46;
  auto centered = transport.withSizeKeepingCentre(bw * 2, transport.getHeight());
  playB.setBounds(centered.removeFromLeft(bw).reduced(3));
  stopB.setBounds(centered.removeFromLeft(bw).reduced(3));
  r.removeFromBottom(6);
  trackBox.setBounds(r.removeFromBottom(28).reduced(1, 2));
  r.removeFromBottom(4);
  timeL.setBounds(r.removeFromBottom(18));
  title.setBounds(r);
}

} // namespace pg
