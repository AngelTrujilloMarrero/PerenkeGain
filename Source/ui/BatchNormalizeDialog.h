#pragma once
#include <JuceHeader.h>
#include "storage/BatchNormalizer.h"
#include "types/Text.h"
#include "ui/FilePicker.h"

namespace pg {

// Ventana de normalizacion por lotes tipo mp3gain: Analizar (mide volumen y
// pico de cada fichero), fijar el nivel objetivo (dB) y Aplicar por pista o
// por album. La salida conserva el formato de entrada.
class BatchNormalizeDialog : public juce::Component {
public:
  BatchNormalizeDialog();
  void paint(juce::Graphics &g) override;
  void resized() override;

private:
  void updateLabels();
  void analyze();
  void apply(bool album);
  void renderReport(const juce::String &title = {});
  void setBusy(bool busy);

  juce::TextButton pickFile{"Archivo..."}, pickFolder{"Carpeta..."},
      pickOut{"Salida..."}, analyzeB{"Analizar"}, applyTrackB{"Aplicar pista"},
      applyAlbumB{PG_T("Aplicar álbum")};
  juce::ToggleButton noClip{"Evitar recorte"};
  juce::Label sourceL, outL, targetL;
  juce::Slider target;
  juce::TextEditor log;
  FilePicker picker;
  juce::File source, outDir;
  std::vector<juce::File> files;
  std::vector<NormalizeAnalysis> analyses;
};

} // namespace pg
