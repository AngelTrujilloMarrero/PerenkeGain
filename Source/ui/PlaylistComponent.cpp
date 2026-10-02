#include "ui/PlaylistComponent.h"

namespace pg {

PlaylistComponent::PlaylistComponent(AudioEngine &e, PlaylistModel &p)
    : engine(e), playlist(p) {
  addAndMakeVisible(list);
  list.setModel(this);
  list.setRowHeight(24);
  list.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xFF0E1014));

  for (auto *b : {&upB, &downB, &removeB, &toAB, &toBB})
    addAndMakeVisible(*b);
  upB.onClick = [this] { moveSelected(-1); };
  downB.onClick = [this] { moveSelected(1); };
  removeB.onClick = [this] { removeSelected(); };
  toAB.onClick = [this] { sendSelected(0); };
  toBB.onClick = [this] { sendSelected(1); };

  playlist.addChangeListener(this);
  startTimerHz(4); // refresca la insignia de deck al cambiar los decks
}

PlaylistComponent::~PlaylistComponent() {
  stopTimer();
  playlist.removeChangeListener(this);
}

void PlaylistComponent::changeListenerCallback(juce::ChangeBroadcaster *) {
  list.updateContent();
}

void PlaylistComponent::timerCallback() {
  if (isShowing())
    list.repaint();
}

int PlaylistComponent::getNumRows() { return playlist.size(); }

int PlaylistComponent::deckOf(const juce::File &f) const {
  for (int d = 0; d < AudioEngine::kDecks; ++d)
    if (engine.getFile(d) == f)
      return d;
  return -1;
}

void PlaylistComponent::paintListBoxItem(int row, juce::Graphics &g, int width,
                                         int height, bool selected) {
  if (row < 0 || row >= playlist.size())
    return;
  g.fillAll(selected ? juce::Colour(0xFF2B3A55) : juce::Colour(0xFF14151A));
  const int badgeW = 34;
  g.setColour(selected ? juce::Colours::white : juce::Colour(0xFFD7DBE0));
  g.setFont(juce::Font(juce::FontOptions(12.0f)));
  g.drawText(juce::String(row + 1) + ". " + playlist.at(row).title,
             juce::Rectangle<int>(6, 0, width - badgeW - 12, height),
             juce::Justification::centredLeft, true);
  const int d = deckOf(playlist.at(row).file);
  g.setColour(d == 0   ? juce::Colour(0xFF4FA3FF)
              : d == 1 ? juce::Colour(0xFFFF7AD9)
                       : juce::Colour(0xFF555B66));
  g.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
  g.drawText(d == 0 ? "A" : d == 1 ? "B" : "-",
             juce::Rectangle<int>(width - badgeW - 6, 0, badgeW, height),
             juce::Justification::centred);
}

juce::var
PlaylistComponent::getDragSourceDescription(const juce::SparseSet<int> &rows) {
  if (rows.size() == 0)
    return {};
  return rows[0];
}

bool PlaylistComponent::isInterestedInDragSource(const SourceDetails &) {
  return true;
}

void PlaylistComponent::itemDropped(const SourceDetails &details) {
  if (!details.description.isInt())
    return;
  const int from = (int)details.description;
  if (from < 0 || from >= playlist.size())
    return;
  auto p = list.getLocalPoint(this, details.localPosition.toInt());
  int to = list.getRowContainingPosition(p.x, p.y);
  if (to < 0)
    to = playlist.size() - 1;
  playlist.move(from, to);
  list.selectRow(to);
}

int PlaylistComponent::selectedEntry() const {
  const int r = list.getSelectedRow();
  return (r >= 0 && r < playlist.size()) ? r : -1;
}

void PlaylistComponent::moveSelected(int delta) {
  const int r = selectedEntry();
  if (r < 0)
    return;
  const int to = juce::jlimit(0, playlist.size() - 1, r + delta);
  playlist.move(r, to);
  list.selectRow(to);
}

void PlaylistComponent::removeSelected() {
  const int r = selectedEntry();
  if (r < 0)
    return;
  playlist.remove(r);
  if (playlist.size() > 0)
    list.selectRow(juce::jmin(r, playlist.size() - 1));
}

void PlaylistComponent::sendSelected(int deck) {
  const int r = selectedEntry();
  if (r < 0)
    return;
  if (onSendToDeck)
    onSendToDeck(deck, playlist.at(r).file);
}

void PlaylistComponent::sendToFreeDeck(int row) {
  if (row < 0 || row >= playlist.size())
    return;
  // Primer deck libre; si no hay, el primero que no esté sonando.
  int deck = -1;
  for (int d = 0; d < AudioEngine::kDecks; ++d)
    if (!engine.hasFile(d)) {
      deck = d;
      break;
    }
  if (deck < 0)
    for (int d = 0; d < AudioEngine::kDecks; ++d)
      if (!engine.isPlaying(d)) {
        deck = d;
        break;
      }
  if (deck < 0)
    deck = 0;
  if (onSendToDeck)
    onSendToDeck(deck, playlist.at(row).file);
}

void PlaylistComponent::listBoxItemDoubleClicked(int row,
                                                const juce::MouseEvent &) {
  sendToFreeDeck(row);
}

void PlaylistComponent::paint(juce::Graphics &g) {
  g.setColour(juce::Colour(0xFF0B0C10));
  g.fillAll();
  g.setColour(juce::Colour(0xFFB9C2D0));
  g.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
  g.drawText(PG_T("Lista de reproducción (arrastra para ordenar, doble "
                    "clic envía al deck)"),
             getLocalBounds().removeFromTop(16).reduced(6, 0),
             juce::Justification::centredLeft);
}

void PlaylistComponent::resized() {
  auto r = getLocalBounds().reduced(6);
  r.removeFromTop(16);
  auto bottom = r.removeFromBottom(28);
  upB.setBounds(bottom.removeFromLeft(76).reduced(2));
  downB.setBounds(bottom.removeFromLeft(76).reduced(2));
  removeB.setBounds(bottom.removeFromLeft(80).reduced(2));
  toAB.setBounds(bottom.removeFromLeft(66).reduced(2));
  toBB.setBounds(bottom.removeFromLeft(66).reduced(2));
  r.removeFromBottom(4);
  list.setBounds(r);
}

} // namespace pg
