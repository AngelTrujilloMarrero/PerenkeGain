#include "ui/UpdateDialog.h"
#include "types/Text.h"

namespace pg {

namespace {
constexpr int kHeaderH = 68;
}

UpdateDialog::UpdateDialog(updater::UpdateInfo updateInfo)
    : info(std::move(updateInfo)) {
  const auto *best = info.assetForThisPlatform();
  if (best != nullptr)
    asset = *best;

  title.setText(PG_T("Actualizaci\u00f3n disponible"), juce::dontSendNotification);
  title.setFont(juce::Font(juce::FontOptions(19.0f, juce::Font::bold)));
  title.setColour(juce::Label::textColourId, juce::Colours::white);
  addAndMakeVisible(title);

  subtitle.setText("PerenkeGain v" + info.latestVersion +
                       PG_T("  \u00b7  tienes la v") + info.currentVersion,
                   juce::dontSendNotification);
  subtitle.setFont(juce::Font(juce::FontOptions(12.5f)));
  subtitle.setColour(juce::Label::textColourId,
                     juce::Colours::white.withAlpha(0.85f));
  addAndMakeVisible(subtitle);

  notesTitle.setText(PG_T("Novedades de la versi\u00f3n ") + info.latestVersion,
                     juce::dontSendNotification);
  notesTitle.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
  addAndMakeVisible(notesTitle);

  notes.setMultiLine(true);
  notes.setReadOnly(true);
  notes.setScrollbarsShown(true);
  notes.setCaretVisible(false);
  notes.setFont(juce::Font(juce::FontOptions(12.0f)));
  notes.setText(info.releaseNotes.isNotEmpty()
                    ? info.releaseNotes
                    : PG_T("Esta versi\u00f3n no incluye notas de cambios."));
  addAndMakeVisible(notes);

  addAndMakeVisible(progress);
  progress.setFraction(0.0);

  status.setFont(juce::Font(juce::FontOptions(11.5f)));
  status.setVisible(true);
  setStatus(asset.downloadUrl.isNotEmpty()
                ? PG_T("Se guardar\u00e1 en tu carpeta de Descargas.")
                : PG_T("No hay instalador para tu sistema: se abrir\u00e1 GitHub."));
  addAndMakeVisible(status);

  downloadBtn.setButtonText(PG_T("Descargar e instalar"));
  downloadBtn.onClick = [this] { startDownload(); };
  addAndMakeVisible(downloadBtn);

  githubBtn.setButtonText(PG_T("Ver en GitHub"));
  githubBtn.onClick = [this] { openReleasePage(); };
  addAndMakeVisible(githubBtn);

  closeBtn.setButtonText(PG_T("M\u00e1s tarde"));
  closeBtn.onClick = [this] { closeDialog(); };
  addAndMakeVisible(closeBtn);

  downloader = std::make_unique<updater::UpdateDownloader>();
}

void UpdateDialog::setStatus(const juce::String &text) {
  status.setText(text, juce::dontSendNotification);
}

void UpdateDialog::startDownload() {
  if (downloading)
    return;
  if (asset.downloadUrl.isEmpty()) {
    openReleasePage();
    return;
  }

  auto downloads = juce::File::getSpecialLocation(
                       juce::File::userHomeDirectory)
                       .getChildFile("Downloads");
  downloads.createDirectory();
  auto target = downloads.getChildFile(asset.name.isNotEmpty()
                                           ? asset.name
                                           : juce::String("PerenkeGain-update"));
  downloading = true;
  downloadBtn.setEnabled(false);
  githubBtn.setEnabled(false);
  downloadBtn.setButtonText(PG_T("Descargando\u2026"));
  setStatus(PG_T("Descargando ") + asset.name + PG_T("\u2026"));

  auto safe = juce::Component::SafePointer<UpdateDialog>(this);
  downloader->start(
      asset, target,
      [safe](double v) {
        if (safe != nullptr)
          safe->progress.setFraction(v);
      },
      [safe](bool ok, juce::File file) {
        if (safe != nullptr)
          safe->onDownloadFinished(ok, file);
      });
}

void UpdateDialog::onDownloadFinished(bool ok, juce::File file) {
  downloading = false;
  downloadBtn.setEnabled(true);
  githubBtn.setEnabled(true);
  downloadBtn.setButtonText(PG_T("Descargar e instalar"));

  if (ok) {
    progress.setFraction(1.0);
    setStatus(PG_T("Completado: ") + file.getFileName() +
              PG_T(". Cierra la app y ejec\u00fatalo para actualizar."));
    file.revealToUser();
  } else {
    setStatus(PG_T("No se pudo descargar. Abriendo GitHub\u2026"));
    openReleasePage();
  }
}

void UpdateDialog::openReleasePage() {
  if (info.releaseUrl.isNotEmpty())
    juce::URL(info.releaseUrl).launchInDefaultBrowser();
  closeDialog();
}

void UpdateDialog::closeDialog() {
  if (auto *dw = findParentComponentOfClass<juce::DialogWindow>())
    dw->exitModalState(0);
}

void UpdateDialog::paint(juce::Graphics &g) {
  auto header = getLocalBounds().removeFromTop(kHeaderH).toFloat();
  juce::ColourGradient grad(juce::Colour(0xFF0B3C5D), header.getTopLeft(),
                            juce::Colour(0xFF17A2B8), header.getBottomRight(),
                            false);
  g.setGradientFill(grad);
  g.fillRect(header);
}

void UpdateDialog::resized() {
  auto r = getLocalBounds();
  auto header = r.removeFromTop(kHeaderH).reduced(16, 8);
  title.setBounds(header.removeFromTop(header.getHeight() - 18));
  subtitle.setBounds(header);

  r = r.reduced(16, 12);
  notesTitle.setBounds(r.removeFromTop(20));
  r.removeFromTop(4);
  auto footer = r.removeFromBottom(74);
  status.setBounds(footer.removeFromTop(20));
  footer.removeFromTop(4);
  progress.setBounds(footer.removeFromTop(16));
  footer.removeFromTop(6);
  auto buttons = footer;
  downloadBtn.setBounds(buttons.removeFromLeft(175).reduced(0, 2));
  githubBtn.setBounds(buttons.removeFromLeft(120).reduced(6, 2));
  closeBtn.setBounds(buttons.reduced(6, 2));
  notes.setBounds(r);
}

} // namespace pg
