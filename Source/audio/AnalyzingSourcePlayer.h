#pragma once
#include <JuceHeader.h>
#include "dsp/BandLevelAnalyzer.h"

namespace pg {

// AudioSourcePlayer que analiza la señal de salida para alimentar los
// medidores de nivel por banda del ecualizador.
class AnalyzingSourcePlayer : public juce::AudioSourcePlayer {
public:
  BandLevelAnalyzer *analyzer = nullptr;

  void audioDeviceAboutToStart(juce::AudioIODevice *device) override {
    juce::AudioSourcePlayer::audioDeviceAboutToStart(device);
    if (analyzer != nullptr && device != nullptr)
      analyzer->prepare(device->getCurrentSampleRate());
  }

  void audioDeviceIOCallbackWithContext(
      const float *const *inputChannelData, int totalNumInputChannels,
      float *const *outputChannelData, int totalNumOutputChannels,
      int numSamples,
      const juce::AudioIODeviceCallbackContext &context) override {
    juce::AudioSourcePlayer::audioDeviceIOCallbackWithContext(
        inputChannelData, totalNumInputChannels, outputChannelData,
        totalNumOutputChannels, numSamples, context);
    if (analyzer != nullptr)
      analyzer->process(outputChannelData, totalNumOutputChannels,
                        numSamples);
  }
};

} // namespace pg
