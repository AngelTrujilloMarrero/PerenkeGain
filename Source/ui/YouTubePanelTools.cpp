#include "ui/YouTubeSearchPanel.h"
#include "storage/ExternalTool.h"
#include "storage/YouTubeDownloader.h"
#include "ui/ToolInstallHint.h"

namespace pg {

// Estado de las herramientas externas (yt-dlp/ffmpeg) y su instalacion
// interna: si falta algo se ofrece el boton de descarga de la propia app.
void YouTubeSearchPanel::refreshToolStatus() {
#if JUCE_ANDROID
  installB.setVisible(false);
  setStatus(PG_T("Build ") + juce::String(__DATE__) + " " + __TIME__ + " - " +
            outDir.getFullPathName());
  return;
#else
  const bool pending = !ToolInstaller::ready();
  installB.setVisible(pending);
  if (pending) {
    setStatus(toolInstallHint());
    return;
  }
  juce::File ytdlp, ffmpeg;
  if (YouTubeDownloader::available(ytdlp, ffmpeg) &&
      ExternalTool::isYtDlpOutdated(ytdlp)) {
    setStatus(ExternalTool::ytDlpUpdateHint());
    return;
  }
  setStatus(PG_T("Listo. Carpeta: ") + outDir.getFullPathName());
#endif
}

void YouTubeSearchPanel::startToolInstall() {
  if (!installB.isVisible() || !installB.isEnabled())
    return;
  installB.setEnabled(false);
  setStatus(PG_T("Descargando herramientas..."));
  auto safe = juce::Component::SafePointer<YouTubeSearchPanel>(this);
  toolInstaller.install(
      [safe](const juce::String &tool, float frac) {
        if (safe != nullptr)
          safe->setStatus(PG_T("Descargando ") + tool + " " +
                          juce::String((int)(frac * 100.0f)) + "%");
      },
      [safe](bool ok, const juce::String &msg) {
        if (safe == nullptr)
          return;
        safe->installB.setEnabled(true);
        if (!ok) {
          safe->setStatus(PG_T("Error: ") + msg);
          return;
        }
        safe->refreshToolStatus();
      });
}

} // namespace pg
