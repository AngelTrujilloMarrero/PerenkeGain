#pragma once
#include <JuceHeader.h>
#include <functional>

namespace pg {

// Selector de fichero o carpeta reutilizable (async, con un FileChooser vivo).
class FilePicker {
public:
  enum class Mode { OpenFile, OpenDirectory };

  void choose(Mode mode, const juce::String &title,
              const juce::String &patterns, const juce::File &startDir,
              std::function<void(const juce::File &)> onResult);

private:
  std::unique_ptr<juce::FileChooser> chooser;
};

} // namespace pg
