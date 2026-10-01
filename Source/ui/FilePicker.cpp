#include "ui/FilePicker.h"

namespace pg {

void FilePicker::choose(Mode mode, const juce::String &title,
                        const juce::String &patterns, const juce::File &startDir,
                        std::function<void(const juce::File &)> onResult) {
  chooser = std::make_unique<juce::FileChooser>(
      title, startDir, mode == Mode::OpenFile ? patterns : juce::String("*"));
  const int flags =
      juce::FileBrowserComponent::openMode |
      (mode == Mode::OpenFile ? juce::FileBrowserComponent::canSelectFiles
                              : juce::FileBrowserComponent::canSelectDirectories);
  chooser->launchAsync(flags, [cb = std::move(onResult)](
                                  const juce::FileChooser &fc) {
    if (cb)
      cb(fc.getResult());
  });
}

void FilePicker::chooseFiles(
    const juce::String &title, const juce::String &patterns,
    const juce::File &startDir,
    std::function<void(const juce::Array<juce::File> &)> onResult) {
  chooser =
      std::make_unique<juce::FileChooser>(title, startDir, patterns);
  const int flags = juce::FileBrowserComponent::openMode |
                    juce::FileBrowserComponent::canSelectFiles |
                    juce::FileBrowserComponent::canSelectMultipleItems;
  chooser->launchAsync(flags, [cb = std::move(onResult)](
                                  const juce::FileChooser &fc) {
    if (cb)
      cb(fc.getResults());
  });
}

} // namespace pg
