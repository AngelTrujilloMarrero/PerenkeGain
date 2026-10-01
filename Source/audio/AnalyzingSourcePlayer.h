#pragma once
#include <JuceHeader.h>
#include "dsp/BandLevelAnalyzer.h"
#include "dsp/Eq31BandProcessor.h"

namespace pg {

// AudioSourcePlayer que aplica el EQ al audio de salida y luego lo analiza
// para alimentar los medidores de nivel por banda del ecualizador.
class AnalyzingSourcePlayer : public juce::AudioSourcePlayer {
public:
  BandLevelAnalyzer *analyzer = nullptr;
  Eq31BandProcessor *eqProcessor = nullptr;

  void audioDeviceAboutToStart(juce::AudioIODevice *device) override {
    juce::AudioSourcePlayer::audioDeviceAboutToStart(device);
    if (device == nullptr)
      return;
    if (analyzer != nullptr)
      analyzer->prepare(device->getCurrentSampleRate());
    if (eqProcessor != nullptr) {
      int ch = juce::jmax(
          1, device->getActiveOutputChannels().countNumberOfSetBits());
      eqProcessor->prepare(device->getCurrentSampleRate(), ch,
                           device->getCurrentBufferSizeSamples());
    }
  }

  void audioDeviceIOCallbackWithContext(
      const float *const *inputChannelData, int totalNumInputChannels,
      float *const *outputChannelData, int totalNumOutputChannels,
      int numSamples,
      const juce::AudioIODeviceCallbackContext &context) override {
    juce::AudioSourcePlayer::audioDeviceIOCallbackWithContext(
        inputChannelData, totalNumInputChannels, outputChannelData,
        totalNumOutputChannels, numSamples, context);
    // EQ real sobre la salida (antes de medir).
    if (eqProcessor != nullptr && totalNumOutputChannels > 0) {
      juce::AudioBuffer<float> buf(outputChannelData, totalNumOutputChannels,
                                   numSamples);
      eqProcessor->process(buf);
    }
    if (analyzer != nullptr)
      analyzer->process(outputChannelData, totalNumOutputChannels,
                        numSamples);
  }
};

} // namespace pg
