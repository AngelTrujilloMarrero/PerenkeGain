#pragma once
#include <JuceHeader.h>

namespace pg {

// Mensaje de instalacion de yt-dlp/ffmpeg segun plataforma.
// Mensaje de instalacion de yt-dlp/ffmpeg segun plataforma. La app tambien
// los descarga sola con el boton "Instalar herramientas".
inline juce::String toolInstallHint() {
#if JUCE_LINUX
  return juce::String("Faltan yt-dlp/ffmpeg: pulsa 'Instalar herramientas' "
                      "(o pipx install yt-dlp; sudo apt install ffmpeg)");
#elif JUCE_MAC
  return juce::String(
      "Faltan yt-dlp/ffmpeg: pulsa 'Instalar herramientas' "
      "(o brew install yt-dlp ffmpeg)");
#elif JUCE_WINDOWS
  return juce::String(
      "Faltan yt-dlp/ffmpeg: pulsa 'Instalar herramientas' "
      "(o winget install yt-dlp; winget install ffmpeg)");
#else
  return juce::String("Faltan yt-dlp/ffmpeg: pulsa 'Instalar herramientas'");
#endif
}

} // namespace pg
