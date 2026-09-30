#pragma once
#include <JuceHeader.h>

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

private:
  juce::File fileName;
  juce::AudioFormatManager formats;
  juce::AudioDeviceManager devices;
  juce::AudioSourcePlayer player;
  juce::AudioTransportSource transport;
  std::unique_ptr<juce::AudioFormatReaderSource> reader;
};

} // namespace pg
