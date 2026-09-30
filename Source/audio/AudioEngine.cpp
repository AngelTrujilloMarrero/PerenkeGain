#include "audio/AudioEngine.h"

namespace pg {

AudioEngine::AudioEngine() {
  formats.registerBasicFormats();
}

void AudioEngine::loadFile(const juce::File &f) {
  if (auto *r = formats.createReaderFor(f)) {
    reader = std::make_unique<juce::AudioFormatReaderSource>(r, true);
    transport.setSource(reader.get(), 0, nullptr, r->sampleRate);
  }
}

void AudioEngine::play() { transport.start(); }
void AudioEngine::stop() { transport.stop(); }

double AudioEngine::getLengthSec() const {
  return transport.getLengthInSeconds();
}

} // namespace pg
