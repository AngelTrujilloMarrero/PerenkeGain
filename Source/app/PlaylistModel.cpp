#include "app/PlaylistModel.h"

namespace pg {

void PlaylistModel::add(const juce::File &file, const juce::String &title) {
  if (!file.existsAsFile())
    return;
  for (int i = 0; i < (int)entries.size(); ++i) {
    if (entries[(size_t)i].file == file) { // ya estaba: solo seleccionar
      select(i);
      return;
    }
  }
  PlaylistEntry e;
  e.file = file;
  e.title = title.isNotEmpty() ? title : file.getFileNameWithoutExtension();
  entries.push_back(e);
  selected = (int)entries.size() - 1;
  sendChangeMessage();
}

void PlaylistModel::addAll(const juce::Array<juce::File> &files) {
  bool added = false;
  for (const auto &f : files) {
    if (!f.existsAsFile())
      continue;
    bool exists = false;
    for (auto &e : entries)
      if (e.file == f) {
        exists = true;
        break;
      }
    if (exists)
      continue;
    entries.push_back({f.getFileNameWithoutExtension(), f});
    added = true;
  }
  if (added) {
    selected = (int)entries.size() - 1;
    sendChangeMessage();
  }
}

juce::File PlaylistModel::selectedFile() const {
  if (selected < 0 || selected >= (int)entries.size())
    return {};
  return entries[(size_t)selected].file;
}

void PlaylistModel::select(int index) {
  if (index < 0 || index >= (int)entries.size() || index == selected)
    return;
  selected = index;
  sendChangeMessage();
}

void PlaylistModel::remove(int index) {
  if (index < 0 || index >= (int)entries.size())
    return;
  entries.erase(entries.begin() + index);
  if (selected == index)
    selected = juce::jmin(index, (int)entries.size() - 1);
  else if (selected > index)
    --selected;
  sendChangeMessage();
}

void PlaylistModel::move(int from, int to) {
  if (from < 0 || from >= (int)entries.size())
    return;
  to = juce::jlimit(0, (int)entries.size() - 1, to);
  if (from == to)
    return;
  PlaylistEntry e = entries[(size_t)from];
  entries.erase(entries.begin() + from);
  entries.insert(entries.begin() + to, e);

  if (selected == from)
    selected = to;
  else if (from < selected && to >= selected)
    --selected;
  else if (from > selected && to <= selected)
    ++selected;
  sendChangeMessage();
}

} // namespace pg
