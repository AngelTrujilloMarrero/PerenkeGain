#include "ui/FilePicker.h"

namespace pg {
namespace {

// En Android el selector devuelve URIs content:// (Storage Access Framework),
// que no son ficheros locales y getResults() descarta. Se copian a un fichero
// temporal para poder abrirlos con AudioFormatManager. En escritorio es identidad.
juce::File importPickedUrl(const juce::URL &url) {
#if JUCE_ANDROID
  if (url.isLocalFile())
    return url.getLocalFile();
  if (url.getScheme() == "content") {
    auto doc = juce::AndroidDocument::fromDocument(url);
    if (!doc.hasValue())
      return {};
    auto in = doc.createInputStream();
    if (in == nullptr)
      return {};
    auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                   .getChildFile("imports");
    if (!dir.isDirectory() && !dir.createDirectory())
      return {};
    const juce::String name = doc.getInfo().getName();
    juce::File dest =
        dir.getChildFile(name.isNotEmpty() ? name : juce::String("audio"));
    dest.deleteFile();
    if (auto out = dest.createOutputStream()) {
      out->writeFromInputStream(*in, -1);
      out->flush();
    }
    return (dest.existsAsFile() && dest.getSize() > 0) ? dest : juce::File{};
  }
  return {};
#else
  return url.isLocalFile() ? url.getLocalFile() : juce::File{};
#endif
}

} // namespace

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
    if (!cb)
      return;
    const auto urls = fc.getURLResults();
    cb(urls.isEmpty() ? juce::File{} : importPickedUrl(urls.getFirst()));
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
    if (!cb)
      return;
    juce::Array<juce::File> files;
    for (const auto &url : fc.getURLResults()) {
      auto f = importPickedUrl(url);
      if (f != juce::File{})
        files.add(f);
    }
    cb(files);
  });
}

} // namespace pg
