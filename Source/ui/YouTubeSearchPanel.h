#pragma once
#include <JuceHeader.h>
#include <memory>
#include "storage/ToolInstaller.h"
#include "storage/YouTubeSearch.h"
#include "types/Text.h"
#include "ui/FilePicker.h"
#include "ui/YouTubeSearchTask.h"
#include "ui/YouTubeTask.h"

namespace pg {

// Buscador de YouTube Music integrado en la ventana principal: busca por
// texto, lista resultados y descarga el elegido (MP3 normalizado).
class YouTubeSearchPanel : public juce::Component,
                           private juce::ListBoxModel {
public:
  YouTubeSearchPanel();
  void paint(juce::Graphics &g) override;
  void resized() override;

  // Se dispara por cada pista descargada y normalizada.
  std::function<void(const juce::File &)> onDownloaded;

private:
  void startSearch();
  void onSearchDone(std::vector<YouTubeSearchItem> found, juce::String error);
  void startDownload();
  void onDownloadDone(juce::Array<juce::File> ok, juce::StringArray errors);
  void clearAll();
  void setResults(std::vector<YouTubeSearchItem> items);
  void setStatus(const juce::String &s);
  // YouTubePanelTools.cpp: estado de yt-dlp/ffmpeg e instalacion interna.
  void refreshToolStatus();
  void startToolInstall();

  int getNumRows() override;
  void paintListBoxItem(int row, juce::Graphics &g, int width, int height,
                        bool rowIsSelected) override;
  void listBoxItemDoubleClicked(int row, const juce::MouseEvent &) override;

  juce::TextEditor query;
  juce::TextButton searchB{"Buscar"}, clearB{"Limpiar"},
      folderB{"Carpeta..."}, downloadB{"Descargar"},
      installB{"Instalar herramientas"};
  juce::ListBox results;
  juce::Label status;
  std::vector<YouTubeSearchItem> items;
  std::unique_ptr<YouTubeSearchTask> searchTask;
  std::unique_ptr<YouTubeTask> downloadTask;
  ToolInstaller toolInstaller;
  int statusRow = -1;
  juce::String rowInfo;
  juce::File outDir;
  FilePicker picker;
};

} // namespace pg
