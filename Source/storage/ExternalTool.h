#pragma once
#include <JuceHeader.h>

namespace pg {

// Localiza ejecutables en el PATH (yt-dlp, ffmpeg, lame...) para las
// herramientas externas del proyecto.
class ExternalTool {
public:
  static juce::File find(const juce::String &name);
};

} // namespace pg
