#pragma once
#include <JuceHeader.h>
#include <functional>
#include <memory>

namespace pg {

// Descarga yt-dlp y ffmpeg (binarios oficiales sin comprimir) a la carpeta
// gestionada por la app, para no pedir winget/brew/apt en el primer arranque.
// Los callbacks llegan al hilo principal de la UI.
class ToolInstaller {
public:
  using Progress = std::function<void(const juce::String &tool, float frac)>;
  using Done = std::function<void(bool ok, const juce::String &message)>;

  // Herramientas pendientes: ausentes o yt-dlp caducado (mas de 180 dias).
  static juce::StringArray pending();
  static bool ready() { return pending().isEmpty(); }

  // Descarga en un hilo lo que este pendiente, una tras otra.
  void install(Progress onProgress, Done onDone);
  void cancel();

  ~ToolInstaller();

private:
  struct State;
  std::shared_ptr<State> state;
};

} // namespace pg
