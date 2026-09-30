#pragma once
#include <JuceHeader.h>
#include "types/AudioTypes.h"
#include "types/EqTypes.h"

namespace pg {

// Estado global UI (no audio realtime). Single source of truth.
struct ApplicationState {
  AudioSpec spec{};
  SilenceParams silence{};
  Eq31State eq{};
  juce::File currentFile{};
  bool filtersBypass = false; // A/B original vs filtrado
};

} // namespace pg
