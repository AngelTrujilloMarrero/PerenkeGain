#pragma once
#include <JuceHeader.h>

namespace pg {

// Mensaje de instalacion de yt-dlp/ffmpeg segun plataforma.
inline juce::String toolInstallHint() {
#if JUCE_LINUX
  return juce::String("Faltan yt-dlp/ffmpeg (pipx install yt-dlp; "
                      "sudo apt install ffmpeg)");
#elif JUCE_MAC
  return juce::String("Faltan yt-dlp/ffmpeg (brew install yt-dlp ffmpeg)");
#elif JUCE_WINDOWS
  return juce::String(
      "Faltan yt-dlp/ffmpeg (winget install yt-dlp; winget install ffmpeg)");
#else
  return juce::String("Faltan yt-dlp/ffmpeg");
#endif
}

} // namespace pg
