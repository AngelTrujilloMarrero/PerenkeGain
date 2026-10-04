#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include "audio/AnalyzingSourcePlayer.h"
#include "dsp/BandLevelAnalyzer.h"
#include "dsp/Eq31BandProcessor.h"
#include "dsp/Leveler.h"

namespace pg {

// Motor de reproduccion estilo mesa DJ: dos decks independientes mezclados con
// un crossfader. El EQ y el nivelador se aplican al resultado de la mezcla.
class AudioEngine {
public:
  static constexpr int kDecks = 2;

  AudioEngine();
  ~AudioEngine();

  // Deck 0 = A, 1 = B.
  void loadFile(int deck, const juce::File &f);
  void clearDeck(int deck); // detiene y descarga la pista
  void play(int deck);
  void stop(int deck);
  void togglePlayPause(int deck);
  void setCurrentPosition(int deck, double sec);
  bool isPlaying(int deck) const;
  bool hasFile(int deck) const;
  double getLengthSec(int deck) const;
  double getPositionSec(int deck) const;
  juce::String getFileName(int deck) const;
  juce::File getFile(int deck) const;
  juce::String getSourceInfo(int deck);

  // crossfader 0 = A, 1 = B (ley de potencia constante)
  void setCrossfader(float f);
  float crossfader() const { return cross; }

  // Volumen master de la mezcla (0..1).
  void setMasterGain(float g) { masterGain.store(juce::jlimit(0.0f, 1.0f, g)); }
  float masterGainValue() const { return masterGain.load(); }

  // Deck activo: el que siguen la onda, los marcadores y la info.
  int activeDeck() const { return active; }
  void setActiveDeck(int d);

  BandLevelAnalyzer &bandAnalyzer() { return bands; }
  // Algun deck reproduciendo (transporte en marcha).
  bool anyDeckPlaying() const {
    for (int d = 0; d < kDecks; ++d)
      if (isPlaying(d))
        return true;
    return false;
  }
  // Carga del hilo de audio (0..1): fraccion del presupuesto del callback
  // usada, solo cuando hay reproduccion real. En pausa el callback corre
  // igual (silencio) y su medida no significa nada, asi que devuelve 0.
  double audioLoad() const {
    return anyDeckPlaying() ? devices.getCpuUsage() : 0.0;
  }
  void setEqState(const Eq31State &s) { eq.setState(s); }
  void setLevelerParams(const LevelerParams &p) { leveler.setParams(p); }
  LevelerParams getLevelerParams() const { return leveler.getParams(); }
  LevelerMeters getLevelerMeters() const { return leveler.getMeters(); }

private:
  struct Deck {
    std::unique_ptr<juce::AudioFormatReaderSource> reader;
    juce::AudioTransportSource transport;
    juce::File file;
  };

  juce::AudioFormatManager formats;
  juce::AudioDeviceManager devices;
  std::array<Deck, kDecks> decks;
  juce::MixerAudioSource mixer;
  AnalyzingSourcePlayer player;
  BandLevelAnalyzer bands;
  Eq31BandProcessor eq;
  Leveler leveler;
  std::atomic<float> masterGain{1.0f};
  float cross = 0.5f;
  int active = 0;
};

} // namespace pg
