#pragma once
#include <JuceHeader.h>

namespace pg {

// Guardado multipista WAV/MP3/FLAC con ID3 + nombre álbum/pista.
class SaveTracksDialog : public juce::Component {
public:
  SaveTracksDialog();
  void resized() override;

private:
  juce::ComboBox format{"WAV", "MP3", "FLAC"};
  juce::TextEditor album, track;
  juce::TextButton save{"Guardar pistas"};
};

} // namespace pg
