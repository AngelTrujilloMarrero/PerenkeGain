#include "ui/DeckComponent.h"

namespace pg {

namespace {
bool isAudioFile(const juce::File &f) {
  const auto ext = f.getFileExtension().toLowerCase();
  return ext == ".wav" || ext == ".mp3" || ext == ".flac" ||
         ext == ".ogg" || ext == ".aiff" || ext == ".aif";
}
} // namespace

// Acepta filas arrastradas desde la lista de reproducción.
bool DeckComponent::isInterestedInDragSource(const SourceDetails &details) {
  if (!details.description.isInt())
    return false;
  const int row = (int)details.description;
  return row >= 0 && row < playlist.size();
}

void DeckComponent::itemDropped(const SourceDetails &details) {
  if (!details.description.isInt())
    return;
  const int row = (int)details.description;
  if (row < 0 || row >= playlist.size())
    return;
  loadIntoDeck(playlist.at(row).file);
  dragOver = false;
  repaint();
}

void DeckComponent::itemDragEnter(const SourceDetails &) {
  dragOver = true;
  repaint();
}

void DeckComponent::itemDragExit(const SourceDetails &) {
  dragOver = false;
  repaint();
}

// Acepta archivos de audio soltados desde el sistema.
bool DeckComponent::isInterestedInFileDrag(const juce::StringArray &files) {
  for (const auto &p : files)
    if (isAudioFile(juce::File(p)))
      return true;
  return false;
}

void DeckComponent::filesDropped(const juce::StringArray &files, int, int) {
  for (const auto &p : files) {
    juce::File f(p);
    if (!isAudioFile(f) || !f.existsAsFile())
      continue;
    playlist.add(f); // si ya estaba, solo la selecciona
    loadIntoDeck(f);
    break;
  }
  dragOver = false;
  repaint();
}

void DeckComponent::fileDragEnter(const juce::StringArray &, int, int) {
  dragOver = true;
  repaint();
}

void DeckComponent::fileDragExit(const juce::StringArray &) {
  dragOver = false;
  repaint();
}

} // namespace pg
