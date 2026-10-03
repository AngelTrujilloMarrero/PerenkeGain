#include "ui/YouTubeSearchPanel.h"
#include "storage/ExternalTool.h"
#include "storage/YouTubeDownloader.h"
#include "ui/ToolInstallHint.h"

namespace pg {
YouTubeSearchPanel::YouTubeSearchPanel() {
  addAndMakeVisible(query);
  query.setFont(juce::Font(juce::FontOptions(13.0f)));
  query.setTextToShowWhenEmpty(PG_T("Buscar en YouTube Music..."),
                               juce::Colours::grey);
  query.onReturnKey = [this] { startSearch(); };

  addAndMakeVisible(searchB);
  searchB.onClick = [this] { startSearch(); };
  addAndMakeVisible(clearB);
  clearB.onClick = [this] { clearAll(); };
  addAndMakeVisible(folderB);
  folderB.onClick = [this] {
    picker.choose(FilePicker::Mode::OpenDirectory, PG_T("Carpeta de descargas"),
                  {}, outDir, [this](const juce::File &d) {
                    if (d != juce::File{})
                      outDir = d;
                  });
  };
  addAndMakeVisible(downloadB);
  downloadB.onClick = [this] { startDownload(); };

  addAndMakeVisible(results);
  results.setModel(this);
  results.setRowHeight(22);

  addAndMakeVisible(status);
  status.setFont(juce::Font(juce::FontOptions(11.0f)));
  status.setColour(juce::Label::textColourId, juce::Colour(0xFF9AA0AC));

#if JUCE_ANDROID
  outDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
               .getChildFile("PerenkeGain");
  installB.setVisible(false);
#else
  outDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
               .getChildFile("MUSICA");
  if (!outDir.isDirectory())
    outDir = juce::File::getSpecialLocation(juce::File::userMusicDirectory);
#endif

  addAndMakeVisible(installB);
  installB.onClick = [this] { startToolInstall(); };
  refreshToolStatus();
}

void YouTubeSearchPanel::setStatus(const juce::String &s) {
  status.setText(s, juce::dontSendNotification);
}

void YouTubeSearchPanel::setResults(std::vector<YouTubeSearchItem> newItems) {
  items = std::move(newItems);
  statusRow = -1;
  rowInfo.clear();
  results.updateContent();
  results.deselectAllRows();
  setStatus(juce::String((int)items.size()) + PG_T(" resultado(s)."));
}

void YouTubeSearchPanel::clearAll() {
  query.clear();
  items.clear();
  statusRow = -1;
  rowInfo.clear();
  results.updateContent();
  results.deselectAllRows();
  setStatus(PG_T("Listo."));
}

void YouTubeSearchPanel::startSearch() {
  const juce::String q = query.getText().trim();
  if (q.isEmpty())
    return;
  juce::File ytdlp, ffmpeg;
  if (!YouTubeDownloader::available(ytdlp, ffmpeg))
    return setStatus(toolInstallHint());
  if (searchTask != nullptr && searchTask->isThreadRunning())
    return;
  searchB.setEnabled(false);
  statusRow = -1;
  rowInfo.clear();
  setStatus(PG_T("Buscando..."));
  auto safe = juce::Component::SafePointer<YouTubeSearchPanel>(this);
  searchTask = std::make_unique<YouTubeSearchTask>(
      q, 3, [safe](std::vector<YouTubeSearchItem> found, juce::String err) {
        if (safe != nullptr)
          safe->onSearchDone(std::move(found), err);
      });
  searchTask->startThread();
}

void YouTubeSearchPanel::onSearchDone(std::vector<YouTubeSearchItem> found,
                                      juce::String error) {
  if (searchTask) {
    searchTask->stopThread(3000);
    searchTask.reset();
  }
  searchB.setEnabled(true);
  if (found.empty())
    setStatus(PG_T("Error: ") + error);
  else
    setResults(std::move(found));
}

void YouTubeSearchPanel::startDownload() {
  const int row = results.getSelectedRow();
  if (row < 0 || row >= (int)items.size())
    return setStatus(PG_T("Elige un resultado y pulsa Descargar."));
  juce::File ytdlp, ffmpeg;
  if (!YouTubeDownloader::available(ytdlp, ffmpeg))
    return setStatus(toolInstallHint());
  if (downloadTask != nullptr && downloadTask->isThreadRunning())
    return;
  if (!outDir.isDirectory())
    outDir.createDirectory();

  downloadB.setEnabled(false);
  statusRow = row;
  rowInfo = PG_T("descargando...");
  results.repaintRow(row);
  setStatus(PG_T("Descargando: ") + items[(size_t)row].title);

  auto safe = juce::Component::SafePointer<YouTubeSearchPanel>(this);
  downloadTask = std::make_unique<YouTubeTask>(
      std::vector<juce::String>{items[(size_t)row].url}, outDir, 89.0f,
      [safe](juce::Array<juce::File> ok, juce::StringArray errors) {
        if (safe != nullptr)
          safe->onDownloadDone(std::move(ok), std::move(errors));
      });
  downloadTask->startThread();
}

void YouTubeSearchPanel::onDownloadDone(juce::Array<juce::File> ok,
                                        juce::StringArray errors) {
  if (downloadTask) {
    downloadTask->stopThread(3000);
    downloadTask.reset();
  }
  downloadB.setEnabled(true);
  const bool good = !ok.isEmpty();
  for (const auto &f : ok)
    if (onDownloaded)
      onDownloaded(f);
  if (statusRow >= 0) {
    rowInfo = good ? PG_T("OK") : PG_T("error");
    results.repaintRow(statusRow);
  }
  setStatus(good ? PG_T("Añadida al reproductor: ") + ok[0].getFileName()
                 : PG_T("Error: ") + (errors.isEmpty()
                                          ? juce::String("desconocido")
                                          : errors[0]));
}

int YouTubeSearchPanel::getNumRows() { return (int)items.size(); }

void YouTubeSearchPanel::paintListBoxItem(int row, juce::Graphics &g, int width,
                                          int height, bool selected) {
  if (row < 0 || row >= (int)items.size())
    return;
  g.fillAll(selected ? juce::Colour(0xFF2B3A55) : juce::Colour(0xFF14151A));
  g.setColour(selected ? juce::Colours::white : juce::Colour(0xFFD7DBE0));
  g.setFont(juce::Font(juce::FontOptions(12.0f)));
  g.drawText(juce::String(row + 1) + ". " + items[(size_t)row].title,
             juce::Rectangle<int>(6, 0, width - 142, height),
             juce::Justification::centredLeft, true);
  if (row == statusRow && rowInfo.isNotEmpty()) {
    g.setColour(juce::Colour(0xFFFFDC00));
    g.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    g.drawText(rowInfo, juce::Rectangle<int>(width - 136, 0, 130, height),
               juce::Justification::centredRight, false);
  }
}

void YouTubeSearchPanel::listBoxItemDoubleClicked(int row,
                                                  const juce::MouseEvent &) {
  results.selectRow(row);
  startDownload();
}

void YouTubeSearchPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colour(0xFF0B0C10));
  g.fillAll();
  g.setColour(juce::Colour(0xFFE23B3B));
  g.drawRect(getLocalBounds(), 2);
}

void YouTubeSearchPanel::resized() {
  auto r = getLocalBounds().reduced(6);
  auto row = r.removeFromTop(28);
  searchB.setBounds(row.removeFromRight(90).reduced(2));
  clearB.setBounds(row.removeFromRight(80).reduced(2));
  folderB.setBounds(row.removeFromRight(110).reduced(2));
  query.setBounds(row.reduced(2));
  auto bottom = r.removeFromBottom(26);
  downloadB.setBounds(bottom.removeFromLeft(120).reduced(2));
  if (installB.isVisible())
    installB.setBounds(bottom.removeFromLeft(176).reduced(2));
  status.setBounds(bottom.reduced(4, 0));
  r.removeFromTop(4);
  results.setBounds(r);
}

} // namespace pg
