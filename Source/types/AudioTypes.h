#pragma once
#include <cstdint>

namespace pg {

// Audio moderno: 32-bit float, 44.1–192 kHz, mono/estéreo.
struct AudioSpec {
  double sampleRate = 44100.0;
  int numChannels = 2;
  int bitDepth = 32;
  bool isFloat = true;
};

struct SilenceParams {
  float thresholdDbFS = -50.0f; // θ silence: -45..-60
  float minSilenceSec = 2.0f;    // τ silence: 1.5..2.5
  float padSec = 0.25f;          // τ pad
  float minTrackSec = 30.0f;     // τ min_track
  bool splitInMiddle = true;
};

struct TrackRegion {
  double startSec = 0.0;
  double endSec = 0.0;
  int index = 0;
};

} // namespace pg
