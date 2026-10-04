#include "audio/AudioEngine.h"
#include "types/Text.h"

namespace pg {

AudioEngine::AudioEngine() {
  formats.registerBasicFormats();
  for (int i = 0; i < kDecks; ++i)
    mixer.addInputSource(&decks[(size_t)i].transport, false);

  auto err = devices.initialiseWithDefaultDevices(0, 2);
  juce::Logger::writeToLog(
      "PG: audio init -> " + (err.isEmpty() ? juce::String("OK") : err));

  // El player empuja la mezcla -> dispositivo en cada callback de audio.
  player.setSource(&mixer);
  player.analyzer = &bands;
  player.eqProcessor = &eq;
  player.leveler = &leveler;
  player.masterGain = &masterGain;
  devices.addAudioCallback(&player);
  setCrossfader(cross);
}

AudioEngine::~AudioEngine() {
  devices.removeAudioCallback(&player);
  player.setSource(nullptr);
  for (auto &d : decks) {
    d.transport.setSource(nullptr);
    d.reader.reset();
  }
}

void AudioEngine::loadFile(int deck, const juce::File &f) {
  if (deck < 0 || deck >= kDecks)
    return;
  auto &d = decks[(size_t)deck];
  d.transport.stop();
  // Detach del reader anterior antes de reemplazarlo (use-after-free).
  d.transport.setSource(nullptr);
  d.reader.reset();
  if (auto *r = formats.createReaderFor(f)) {
    d.file = f;
    d.reader = std::make_unique<juce::AudioFormatReaderSource>(r, true);
    d.transport.setSource(d.reader.get(), 0, nullptr, r->sampleRate);
    juce::Logger::writeToLog("PG: loadFile deck " + juce::String(deck) +
                             " OK " + f.getFullPathName());
  } else {
    juce::Logger::writeToLog("PG: loadFile deck " + juce::String(deck) +
                             " FAIL " + f.getFullPathName());
  }
}

void AudioEngine::clearDeck(int deck) {
  if (deck < 0 || deck >= kDecks)
    return;
  auto &d = decks[(size_t)deck];
  d.transport.stop();
  d.transport.setSource(nullptr);
  d.reader.reset();
  d.file = juce::File{};
}

void AudioEngine::play(int deck) {
  juce::Logger::writeToLog("PG: play deck " + juce::String(deck) +
                           " hasFile=" +
                           juce::String(hasFile(deck) ? 1 : 0));
  if (deck >= 0 && deck < kDecks && hasFile(deck))
    decks[(size_t)deck].transport.start();
}

void AudioEngine::stop(int deck) {
  if (deck >= 0 && deck < kDecks)
    decks[(size_t)deck].transport.stop();
}

void AudioEngine::togglePlayPause(int deck) {
  if (deck < 0 || deck >= kDecks)
    return;
  if (decks[(size_t)deck].transport.isPlaying())
    stop(deck);
  else
    play(deck);
}

void AudioEngine::setCurrentPosition(int deck, double sec) {
  if (deck < 0 || deck >= kDecks)
    return;
  auto &d = decks[(size_t)deck];
  if (d.reader == nullptr)
    return;
  auto *r = d.reader->getAudioFormatReader();
  d.transport.setNextReadPosition((juce::int64)(sec * r->sampleRate));
}

bool AudioEngine::isPlaying(int deck) const {
  return deck >= 0 && deck < kDecks &&
         decks[(size_t)deck].transport.isPlaying();
}

bool AudioEngine::hasFile(int deck) const {
  return deck >= 0 && deck < kDecks &&
         decks[(size_t)deck].reader != nullptr;
}

double AudioEngine::getLengthSec(int deck) const {
  if (deck < 0 || deck >= kDecks)
    return 0.0;
  return decks[(size_t)deck].transport.getLengthInSeconds();
}

double AudioEngine::getPositionSec(int deck) const {
  if (deck < 0 || deck >= kDecks)
    return 0.0;
  return decks[(size_t)deck].transport.getCurrentPosition();
}

juce::String AudioEngine::getFileName(int deck) const {
  if (deck < 0 || deck >= kDecks)
    return {};
  return decks[(size_t)deck].file.getFileName();
}

juce::File AudioEngine::getFile(int deck) const {
  if (deck < 0 || deck >= kDecks)
    return {};
  return decks[(size_t)deck].file;
}

juce::String AudioEngine::getSourceInfo(int deck) {
  if (deck < 0 || deck >= kDecks || decks[(size_t)deck].reader == nullptr)
    return {};
  auto *r = decks[(size_t)deck].reader->getAudioFormatReader();
  if (r == nullptr)
    return {};
  juce::String ch = r->numChannels > 1 ? PG_T("estéreo") : "mono";
  return juce::String(r->getFormatName()) + "; " +
         juce::String((int)r->sampleRate) + " Hz; " + ch;
}

void AudioEngine::setCrossfader(float f) {
  cross = juce::jlimit(0.0f, 1.0f, f);
  const float a = std::cos(cross * juce::MathConstants<float>::halfPi);
  const float b = std::sin(cross * juce::MathConstants<float>::halfPi);
  decks[0].transport.setGain(a);
  decks[1].transport.setGain(b);
}

void AudioEngine::setActiveDeck(int d) {
  active = juce::jlimit(0, kDecks - 1, d);
}

} // namespace pg
