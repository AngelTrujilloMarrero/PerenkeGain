#include "ui/DeckComponent.h"

namespace pg {

namespace {
juce::String audioPatterns() {
  return "*.wav;*.mp3;*.flac;*.ogg;*.aiff;*.aif";
}
} // namespace

// Cabecera del deck: boton para cargar una cancion y boton X para
// vaciarlo (sonando o no).
void DeckComponent::setupButtons() {
  clearB.setButtonText(PG_T("X"));
  clearB.setEnabled(false);
  addAndMakeVisible(clearB);
  clearB.onClick = [this] { clearFromDeck(); };

  loadB.setButtonText(PG_T("Cargar"));
  addAndMakeVisible(loadB);
  loadB.onClick = [this] { chooseFileToLoad(); };
}

// Vacia el deck desde el boton X: detiene y descarga la pista, y si era
// la activa cede el turno al otro deck si este tiene pista.
void DeckComponent::clearFromDeck() {
  if (!engine.hasFile(index))
    return;
  releaseDeck();
  if (onActivated)
    onActivated(engine.activeDeck());
}

// Libera el deck (pista reproducida hasta el final o quitada a mano).
void DeckComponent::releaseDeck() {
  const bool wasActive = engine.activeDeck() == index;
  engine.clearDeck(index);
  loadedFile = juce::File{};
  updateTitle();
  syncSelection();
  timeL.setText("--:-- / --:--", juce::dontSendNotification);
  playB.setIcon(TransportButton::Icon::Play);
  if (wasActive) {
    const int other = 1 - index;
    if (engine.hasFile(other))
      engine.setActiveDeck(other);
  }
}

// Carga una cancion elegida en el explorador. Si ya habia otra en este
// deck la sustituye (el motor la descarga y carga la nueva) y, si no
// estaba en la lista, se anade al final de la reproduccion.
void DeckComponent::chooseFileToLoad() {
  const juce::File start =
      loadedFile == juce::File{} ? juce::File{} : loadedFile.getParentDirectory();
  picker.choose(FilePicker::Mode::OpenFile, PG_T("Elegir cancion"),
                audioPatterns(), start, [this](const juce::File &f) {
                  if (f == juce::File{})
                    return;
                  playlist.add(f);
                  loadIntoDeck(f);
                });
}

} // namespace pg
