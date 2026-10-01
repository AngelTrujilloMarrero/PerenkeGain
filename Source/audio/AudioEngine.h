#pragma once
#include <JuceHeader.h>
#include "audio/AnalyzingSourcePlayer.h"
#include "dsp/BandLevelAnalyzer.h"
#include "dsp/Eq31BandProcessor.h"

namespace pg {

// Motor playback pasivo (solo editor, sin captura).
// Conecta AudioTransportSource -> AudioSourcePlayer -> tarjeta de sonido,
// sin eso transport.start() no emite sonido porque nadie empuja el audio.
class AudioEngine {
public:
  AudioEngine();
  ~AudioEngine();
  void loadFile(const juce::File &f);
  void play();
  void stop();
  void togglePlayPause();
  void setCurrentPosition(double sec);
  bool isPlaying() const;
  bool hasFile() const { return reader != nullptr; }
  double getLengthSec() const;
  double getPositionSec() const;
  juce::String getFileName() const { return fileName.getFileName(); }
  juce::String getSourceInfo();

  // Medidores por banda del ecualizador (alimentados por la salida).
  BandLevelAnalyzer &bandAnalyzer() { return bands; }

  // Estado del EQ que se aplica al audio en reproducción (hilo UI -> audio).
  void setEqState(const Eq31State &s) { eq.setState(s); }

private:
  juce::File fileName;
  juce::AudioFormatManager formats;
  juce::AudioDeviceManager devices;
  AnalyzingSourcePlayer player;
  juce::AudioTransportSource transport;
  std::unique_ptr<juce::AudioFormatReaderSource> reader;
  BandLevelAnalyzer bands;
  Eq31BandProcessor eq;
};

} // namespace pg
