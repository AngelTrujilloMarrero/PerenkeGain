#pragma once
#include <JuceHeader.h>

namespace pg {

// Escribe un buffer en el mismo formato del fichero de origen. WAV/AIFF/FLAC/
// OGG se codifican con JUCE; MP3 requiere un codificador externo (lame o
// ffmpeg) presente en el PATH.
class AudioEncoder {
public:
  static bool write(const juce::File &out, const juce::AudioBuffer<float> &buf,
                    double sampleRate, const juce::String &extension,
                    juce::String &error);
};

} // namespace pg
