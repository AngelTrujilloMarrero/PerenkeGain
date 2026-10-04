#pragma once
#include <JuceHeader.h>
#include <atomic>
#include "dsp/BandLevelAnalyzer.h"
#include "dsp/Eq31BandProcessor.h"
#include "dsp/Leveler.h"

namespace pg {

// AudioSourcePlayer que aplica el EQ, el nivelador y el volumen master al
// audio de salida y luego lo analiza para los medidores.
class AnalyzingSourcePlayer : public juce::AudioSourcePlayer {
public:
  BandLevelAnalyzer *analyzer = nullptr;
  Eq31BandProcessor *eqProcessor = nullptr;
  Leveler *leveler = nullptr;
  std::atomic<float> *masterGain = nullptr;

  void audioDeviceAboutToStart(juce::AudioIODevice *device) override {
    juce::AudioSourcePlayer::audioDeviceAboutToStart(device);
    if (device == nullptr)
      return;
    if (analyzer != nullptr)
      analyzer->prepare(device->getCurrentSampleRate());
    int ch =
        juce::jmax(1, device->getActiveOutputChannels().countNumberOfSetBits());
    if (eqProcessor != nullptr)
      eqProcessor->prepare(device->getCurrentSampleRate(), ch,
                           device->getCurrentBufferSizeSamples());
    if (leveler != nullptr)
      leveler->prepare(device->getCurrentSampleRate(), ch,
                       device->getCurrentBufferSizeSamples());
  }

  void audioDeviceIOCallbackWithContext(
      const float *const *inputChannelData, int totalNumInputChannels,
      float *const *outputChannelData, int totalNumOutputChannels,
      int numSamples,
      const juce::AudioIODeviceCallbackContext &context) override {
    juce::AudioSourcePlayer::audioDeviceIOCallbackWithContext(
        inputChannelData, totalNumInputChannels, outputChannelData,
        totalNumOutputChannels, numSamples, context);
    // EQ + nivelador + master sobre la salida (antes de medir).
    if (totalNumOutputChannels > 0 &&
        (eqProcessor != nullptr || leveler != nullptr ||
         masterGain != nullptr)) {
      juce::AudioBuffer<float> buf(outputChannelData, totalNumOutputChannels,
                                   numSamples);
      if (eqProcessor != nullptr)
        eqProcessor->process(buf);
      if (leveler != nullptr)
        leveler->process(buf);
      if (masterGain != nullptr)
        buf.applyGain(juce::jlimit(0.0f, 1.0f, masterGain->load()));
    }
    if (analyzer != nullptr)
      analyzer->process(outputChannelData, totalNumOutputChannels,
                        numSamples);
  }
};

} // namespace pg
