#include "storage/AudioFileLoader.h"
#include <limits>

namespace pg {

bool AudioFileLoader::load(const juce::File &file, juce::AudioBuffer<float> &out,
                           double &sampleRate) {
  if (!file.existsAsFile())
    return false;
  juce::AudioFormatManager formats;
  formats.registerBasicFormats();
  std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
  if (reader == nullptr || reader->lengthInSamples <= 0)
    return false;

  const int len = (int)juce::jmin<juce::int64>(reader->lengthInSamples,
                                               std::numeric_limits<int>::max());
  out.setSize((int)reader->numChannels, len);
  out.clear();
  reader->read(&out, 0, len, 0, true, true);
  sampleRate = reader->sampleRate;
  return true;
}

} // namespace pg
