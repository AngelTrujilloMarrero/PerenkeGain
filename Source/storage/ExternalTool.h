#pragma once
#include <JuceHeader.h>

namespace pg {

// Localiza ejecutables en el PATH (yt-dlp, ffmpeg, lame...) para las
// herramientas externas del proyecto.
class ExternalTool {
public:
  static juce::File find(const juce::String &name);
  // Versión del ejecutable ("--version", primera línea) o vacío si falla.
  static juce::String toolVersion(const juce::File &tool);
  // yt-dlp caduca rápido: avisa si su versión (YYYY.MM.DD) tiene más de
  // 180 días. Si no se puede averiguar, no bloquea (devuelve false).
  static bool isYtDlpOutdated(const juce::File &ytdlp,
                              juce::String *foundVersion = nullptr);
  // Cómo actualizar yt-dlp según la plataforma.
  static juce::String ytDlpUpdateHint();
};

} // namespace pg
