#include "ui/FilePicker.h"

namespace pg {
namespace {

// En Android el selector devuelve URIs content:// (Storage Access Framework).
// JUCE las resuelve a una ruta /storage que la app no puede abrir (scoped
// storage), asi que copiamos el contenido a un fichero temporal propio usando
// el ContentResolver (WebInputStream), que si tiene el permiso del selector.
// En escritorio es identidad.
juce::File importPickedUrl(const juce::URL &url) {
#if JUCE_ANDROID
  if (url.getScheme() != "content")
    return url.isLocalFile() ? url.getLocalFile() : juce::File{};

  juce::WebInputStream in(url, false);
  if (!in.connect(nullptr))
    return {};

  auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                 .getChildFile("imports");
  if (!dir.isDirectory() && !dir.createDirectory())
    return {};

  juce::String name = url.getFileName();
  if (name.isEmpty())
    name = "audio";
  juce::File dest = dir.getChildFile(name);
  dest.deleteFile();
  if (auto out = dest.createOutputStream()) {
    out->writeFromInputStream(in, -1);
    out->flush();
  }
  if (!dest.existsAsFile() || dest.getSize() == 0)
    return {};
  juce::Logger::writeToLog("PG: imported " + dest.getFullPathName());
  return dest;
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
