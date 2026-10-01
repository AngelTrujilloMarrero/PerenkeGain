#include "audio/AudioEngine.h"
#include "types/Text.h"

namespace pg {

AudioEngine::AudioEngine() {
  formats.registerBasicFormats();

  auto err = devices.initialiseWithDefaultDevices(0, 2);
  juce::ignoreUnused(err);

  // El player empuja transporte -> dispositivo en cada callback de audio.
  player.setSource(&transport);
  player.analyzer = &bands;
  player.eqProcessor = &eq;
  devices.addAudioCallback(&player);
}

AudioEngine::~AudioEngine() {
  devices.removeAudioCallback(&player);
  player.setSource(nullptr);
  // Detach del reader ANTES de que el miembro 'reader' se destruya: si no,
  // ~AudioTransportSource libera un PositionableAudioSource ya borrado
  // (use-after-free al cerrar con un fichero cargado).
  transport.setSource(nullptr);
}

void AudioEngine::loadFile(const juce::File &f) {
  stop();
  // Detach del reader anterior antes de reemplazarlo (mismo use-after-free).
  transport.setSource(nullptr);
  reader.reset();
  if (auto *r = formats.createReaderFor(f)) {
    fileName = f;
    reader = std::make_unique<juce::AudioFormatReaderSource>(r, true);
    transport.setSource(reader.get(), 0, nullptr, r->sampleRate);
  }
}

void AudioEngine::setCurrentPosition(double sec) {
  if (reader == nullptr)
    return;
  auto *r = reader->getAudioFormatReader();
  transport.setNextReadPosition((juce::int64)(sec * r->sampleRate));
}

juce::String AudioEngine::getSourceInfo() {
  if (reader == nullptr)
    return {};
  auto *r = reader->getAudioFormatReader();
  if (r == nullptr)
    return {};
  juce::String ch = r->numChannels > 1 ? PG_T("estéreo") : "mono";
  return juce::String(r->getFormatName()) + "; " +
         juce::String((int)r->sampleRate) + " Hz; " + ch;
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
