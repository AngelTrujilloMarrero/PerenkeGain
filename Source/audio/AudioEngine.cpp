#include "audio/AudioEngine.h"

namespace pg {

AudioEngine::AudioEngine() {
  formats.registerBasicFormats();

  auto err = devices.initialiseWithDefaultDevices(0, 2);
  juce::ignoreUnused(err);

  // El player empuja transporte -> dispositivo en cada callback de audio.
  player.setSource(&transport);
  devices.addAudioCallback(&player);
}

AudioEngine::~AudioEngine() {
  devices.removeAudioCallback(&player);
  player.setSource(nullptr);
}

void AudioEngine::loadFile(const juce::File &f) {
  stop();
  if (auto *r = formats.createReaderFor(f)) {
    reader = std::make_unique<juce::AudioFormatReaderSource>(r, true);
    transport.setSource(reader.get(), 0, nullptr, r->sampleRate);
  }
}

void AudioEngine::play() {
  if (reader != nullptr)
    transport.start();
}

void AudioEngine::stop() { transport.stop(); }

void AudioEngine::togglePlayPause() {
  if (transport.isPlaying())
    stop();
  else
    play();
}

bool AudioEngine::isPlaying() const { return transport.isPlaying(); }

double AudioEngine::getLengthSec() const {
  return transport.getLengthInSeconds();
}

double AudioEngine::getPositionSec() const {
  return transport.getCurrentPosition();
}

} // namespace pg
