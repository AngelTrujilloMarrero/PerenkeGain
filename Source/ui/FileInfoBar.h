#pragma once
#include <JuceHeader.h>
#include "types/Text.h"

namespace pg {

// Fila de datos estilo original: h:m:s | Duración total | Posición |
// Calidad de grabación. Texto monoespaciado sobre gris.
class FileInfoBar : public juce::Component {
public:
  void setFile(const juce::String &name, double totalSec);
  void setPosition(double sec);
  void setQuality(const juce::String &q);
  void paint(juce::Graphics &g) override;

private:
  juce::String fileName, quality = PG_T("MP3; 44100 Hz; estéreo");
  double total = 0.0, pos = 0.0;
  static juce::String timeStr(double sec);
};

} // namespace pg
