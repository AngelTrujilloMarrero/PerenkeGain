#pragma once
#include <JuceHeader.h>

namespace pg {

// Lee un fichero de audio completo a memoria para análisis/exportación
// offline (no toca el reproductor).
class AudioFileLoader {
public:
  static bool load(const juce::File &file, juce::AudioBuffer<float> &out,
                   double &sampleRate);
};

} // namespace pg
