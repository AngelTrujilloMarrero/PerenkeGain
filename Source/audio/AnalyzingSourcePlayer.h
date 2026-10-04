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
    sampleRate = device->getCurrentSampleRate();
    juce::Logger::writeToLog("PG audio: sr=" + juce::String(sampleRate) +
                             " buf=" +
                             juce::String(device->getCurrentBufferSizeSamples()));
    if (analyzer != nullptr)
      analyzer->prepare(sampleRate);
    int ch =
        juce::jmax(1, device->getActiveOutputChannels().countNumberOfSetBits());
    if (eqProcessor != nullptr)
      eqProcessor->prepare(sampleRate, ch,
                           device->getCurrentBufferSizeSamples());
    if (leveler != nullptr)
      leveler->prepare(sampleRate, ch,
                       device->getCurrentBufferSizeSamples());
  }

  void audioDeviceIOCallbackWithContext(
      const float *const *inputChannelData, int totalNumInputChannels,
      float *const *outputChannelData, int totalNumOutputChannels,
      int numSamples,
      const juce::AudioIODeviceCallbackContext &context) override {
    const auto t0 = juce::Time::getHighResolutionTicks();
    juce::AudioSourcePlayer::audioDeviceIOCallbackWithContext(
        inputChannelData, totalNumInputChannels, outputChannelData,
        totalNumOutputChannels, numSamples, context);
    const auto t1 = juce::Time::getHighResolutionTicks();

    auto t = t1;
    if (totalNumOutputChannels > 0 &&
        (eqProcessor != nullptr || leveler != nullptr ||
         masterGain != nullptr)) {
      juce::AudioBuffer<float> buf(outputChannelData, totalNumOutputChannels,
                                   numSamples);
      if (eqProcessor != nullptr)
        eqProcessor->process(buf);
      const auto tAfterEq = juce::Time::getHighResolutionTicks();
      if (leveler != nullptr)
        leveler->process(buf);
      const auto tAfterLev = juce::Time::getHighResolutionTicks();
      if (masterGain != nullptr)
        buf.applyGain(juce::jlimit(0.0f, 1.0f, masterGain->load()));
      t = tAfterLev;
      accEq += juce::Time::highResolutionTicksToSeconds(tAfterEq - t1);
      accLev += juce::Time::highResolutionTicksToSeconds(tAfterLev - tAfterEq);
    }
    if (analyzer != nullptr) {
      analyzer->process(outputChannelData, totalNumOutputChannels, numSamples);
      const auto tAfter = juce::Time::getHighResolutionTicks();
      accAna += juce::Time::highResolutionTicksToSeconds(tAfter - t);
      t = tAfter;
    }

    accBase += juce::Time::highResolutionTicksToSeconds(t1 - t0);
    const double period =
        sampleRate > 0.0 ? (double)numSamples / sampleRate : 0.0;
    accPeriod += period;
    if (++profCount >= 250) {
      juce::Logger::writeToLog(
          "PG prof(ms): base=" + juce::String(accBase * 1000.0, 2) +
          " eq=" + juce::String(accEq * 1000.0, 2) +
          " lev=" + juce::String(accLev * 1000.0, 2) +
          " ana=" + juce::String(accAna * 1000.0, 2) +
          " tot=" + juce::String((accBase + accEq + accLev + accAna) * 1000.0,
                                 2) +
          " period=" + juce::String(accPeriod * 1000.0, 2));
      accBase = accEq = accLev = accAna = accPeriod = 0.0;
      profCount = 0;
    }
  }

private:
  double sampleRate = 48000.0;
  double accBase = 0.0, accEq = 0.0, accLev = 0.0, accAna = 0.0, accPeriod = 0.0;
  int profCount = 0;
};

} // namespace pg
