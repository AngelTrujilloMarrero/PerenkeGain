#pragma once
#include <JuceHeader.h>
#include "types/AudioTypes.h"

namespace pg {

// Diálogo splitter: umbral, duración silencio, min pista, modo corte.
class TrackSplitterDialog : public juce::Component {
public:
  TrackSplitterDialog();
  SilenceParams getParams() const;
  void resized() override;

private:
  juce::Slider threshold{-60.0, -45.0}, minSilence{0.5, 5.0}, minTrack{5.0, 120.0};
  juce::ToggleButton middle{"Cortar en medio del silencio"};
};

} // namespace pg
