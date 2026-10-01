#pragma once
#include <JuceHeader.h>
#include <vector>

namespace pg {

struct PlaylistEntry {
  juce::String title;
  juce::File file;
};

// Lista de reproduccion del reproductor: canciones locales y descargadas de
// YouTube. Notifica los cambios (altas y seleccion).
class PlaylistModel : public juce::ChangeBroadcaster {
public:
  void add(const juce::File &file, const juce::String &title = {});
  void addAll(const juce::Array<juce::File> &files);

  int size() const { return (int)entries.size(); }
  bool empty() const { return entries.empty(); }
  const PlaylistEntry &at(int i) const { return entries[(size_t)i]; }
  int selectedIndex() const { return selected; }
  juce::File selectedFile() const;
  void select(int index);

  void remove(int index);
  void move(int from, int to);

private:
  std::vector<PlaylistEntry> entries;
  int selected = -1;
};

} // namespace pg
