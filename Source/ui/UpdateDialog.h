#pragma once
#include <JuceHeader.h>
#include <memory>
#include "updater/UpdateInfo.h"
#include "updater/UpdateDownloader.h"
#include "ui/CyanProgressBar.h"

namespace pg {

// Dialogo "hay una version nueva": muestra las novedades y descarga el
// instalador de la plataforma actual (o abre la release si no hay binario).
class UpdateDialog : public juce::Component {
public:
  explicit UpdateDialog(updater::UpdateInfo info);

  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  void startDownload();
  void openReleasePage();
  void closeDialog();
  void onDownloadFinished(bool ok, juce::File file);
  void setStatus(const juce::String &text);

  updater::UpdateInfo info;
  updater::UpdateAsset asset;

  juce::Label title, subtitle, notesTitle, status;
  juce::TextEditor notes;
  CyanProgressBar progress;
  juce::TextButton downloadBtn, githubBtn, closeBtn;

  std::unique_ptr<updater::UpdateDownloader> downloader;
  bool downloading = false;
};

} // namespace pg
