#pragma once
#include <JuceHeader.h>

namespace pg {

// Localiza ejecutables (yt-dlp, ffmpeg, lame...): primero en las carpetas
// propias de la app (bundle junto al ejecutable y descargas del instalador
// interno) y despues en el PATH.
class ExternalTool {
public:
  static juce::File find(const juce::String &name);
  // Carpeta gestionada por la app (ToolInstaller descarga aqui).
  static juce::File managedToolsDir();
  // Carpetas propias con prioridad: bundle (junto al exe) y la gestionada.
  static juce::Array<juce::File> localToolsDirs();
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
