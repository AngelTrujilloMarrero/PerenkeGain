#pragma once
#include <JuceHeader.h>

namespace pg {

// Guardado multipista WAV/MP3/FLAC con ID3 + nombre álbum/pista.
class SaveTracksDialog : public juce::Component {
public:
  SaveTracksDialog();
  void resized() override;

  // formatId: 1=WAV 32-float, 2=MP3 320k, 3=FLAC.
  std::function<void(int formatId, const juce::String &album,
                     const juce::String &track)>
      onSave;

private:
  juce::ComboBox format;
  juce::TextEditor album, track;
  juce::TextButton save{"Guardar pistas"};
};

} // namespace pg
