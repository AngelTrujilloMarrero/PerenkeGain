#include "app/EditorRootComponent.h"

namespace pg {

EditorRootComponent::EditorRootComponent() {
  addAndMakeVisible(markers);
  addAndMakeVisible(wave);
  addAndMakeVisible(transport);
  addAndMakeVisible(advanced);
  addAndMakeVisible(openButton);

  openButton.onClick = [this] { openFile(); };
  transport.play.onClick = [this] { engine.play(); };
  transport.stop.onClick = [this] { engine.stop(); };
}

void EditorRootComponent::openFile() {
  chooser = std::make_unique<juce::FileChooser>(
      "Abrir audio", juce::File{}, "*.wav;*.mp3;*.flac;*.ogg");
  chooser->launchAsync(
      juce::FileBrowserComponent::openMode |
          juce::FileBrowserComponent::canSelectFiles,
      [this](const juce::FileChooser &fc) {
        auto f = fc.getResult();
        if (f != juce::File{}) {
          engine.loadFile(f);
          wave.openFile(f);
          markers.setRegions({});
        }
      });
}

void EditorRootComponent::resized() {
  auto b = getLocalBounds();
  auto top = b.removeFromTop(36);
  openButton.setBounds(top.removeFromLeft(180).reduced(4));
  markers.setBounds(top.reduced(0, 4));
  auto right = b.removeFromRight(230);
  advanced.setBounds(right.reduced(4));
  auto bottom = b.removeFromBottom(64);
  transport.setBounds(bottom.reduced(4));
  wave.setBounds(b.reduced(4));
}

} // namespace pg
