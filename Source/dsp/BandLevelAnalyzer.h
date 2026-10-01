#pragma once
#include <JuceHeader.h>
#include <array>
#include <atomic>
#include "types/EqTypes.h"

namespace pg {

// Nivel RMS por banda (31 bandas 1/3 octava) para los medidores del EQ.
// Se alimenta en el hilo de audio (process) y la UI lee levelDb().
class BandLevelAnalyzer {
public:
  static constexpr int kBands = (int)kEq31Freqs.size(); // 31
  void prepare(double sampleRate);
  void process(const float *const *channels, int numChannels,
               int numSamples);
  float levelDb(int band) const;        // -60..0 dB (lectura de la UI)
  float masterLevelDb(int channel) const; // nivel general por canal (L/R)

private:
  using Filter = juce::dsp::IIR::Filter<float>;
  std::array<std::array<Filter, kBands>, 2> filters; // [canal][banda]
  std::array<std::atomic<float>, kBands> levels{};
  std::array<std::atomic<float>, 2> masterLv{};
  double rate = 0.0;
};

} // namespace pg
