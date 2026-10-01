#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include <vector>
#include "dsp/KWeightingFilter.h"

namespace pg {

// Medidor de sonoridad en tiempo real (ITU-R BS.1770 / EBU R128) por bloques
// de 100 ms con ventana deslizante: LUFS momentaneo (400 ms) y de corto
// plazo (3 s). Lee en el hilo de audio; la UI consulta los atomicos.
class LoudnessMeterRT {
public:
  static constexpr int kHops = 30;          // 30 x 100 ms = 3 s
  static constexpr int kMomentaryHops = 4;  // 4 x 100 ms = 400 ms

  void prepare(double sampleRate, int numChannels);
  void reset();
  void process(const juce::AudioBuffer<float> &buf);

  float momentaryLufs() const { return momentary.load(); }
  float shortTermLufs() const { return shortTerm.load(); }
  float peakDb() const { return peak.load(); }

private:
  void flushHop();
  float windowLufs(int hops) const;

  KWeightingFilter kFilter;
  double rate = 44100.0;
  int channels = 2;
  int hopSamples = 4410;
  int hopFilled = 0;
  int hopCount = 0;
  int hopIndex = 0;
  std::vector<double> hopSumSq;
  std::vector<std::array<double, kHops>> ring;
  std::atomic<float> momentary{-70.0f}, shortTerm{-70.0f}, peak{-70.0f};
  float blockPeak = 0.0f;
};

} // namespace pg
