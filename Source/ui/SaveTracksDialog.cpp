#include "ui/SaveTracksDialog.h"
#include "types/Text.h"

namespace pg {

SaveTracksDialog::SaveTracksDialog() {
  format.addItem("WAV 32-float", 1);
  format.addItem("MP3 320k", 2);
  format.addItem("FLAC", 3);
  format.setSelectedId(1);
  addAndMakeVisible(format);
  addAndMakeVisible(album);
  addAndMakeVisible(track);
  addAndMakeVisible(save);
  album.setTextToShowWhenEmpty(PG_T("Álbum"), juce::Colours::grey);
  track.setTextToShowWhenEmpty("Pista", juce::Colours::grey);

  save.onClick = [this] {
    if (onSave)
      onSave(format.getSelectedId(), album.getText(), track.getText());
    if (auto *dw = findParentComponentOfClass<juce::DialogWindow>())
      dw->exitModalState(0);
  };
}

void SaveTracksDialog::resized() {
  auto r = getLocalBounds();
  format.setBounds(r.removeFromTop(30).reduced(4));
  album.setBounds(r.removeFromTop(30).reduced(4));
  track.setBounds(r.removeFromTop(30).reduced(4));
  save.setBounds(r.removeFromTop(30).reduced(4));
}

} // namespace pg
