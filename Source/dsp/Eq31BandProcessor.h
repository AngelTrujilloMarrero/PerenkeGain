#pragma once
#include <JuceHeader.h>
#include <array>
#include "dsp/Biquad.h"
#include "types/EqTypes.h"

namespace pg {

// EQ gráfica 31 bandas 1/3 octava, independiente por canal (mono/estéreo).
// Las bandas aplican gainDb * intensity; "intensidad general" es la ganancia
// maestra de salida (0..1) aplicada al final de la cadena.
class Eq31BandProcessor {
public:
  void prepare(double sr, int ch, int blockSize);
  void setState(const Eq31State &s);
  void process(juce::AudioBuffer<float> &buf);
  // Limpia el estado de los filtros (al pasar a silencio).
  void resetState();

private:
  static constexpr int kMaxChannels = 2;
  using Filter = Biquad;
  Eq31State state{};
  std::array<std::array<Filter, 31>, kMaxChannels> filters; // [canal][banda]
  double sampleRate = 44100.0;
  int activeChannels = 2;
  juce::SpinLock lock; // UI escribe coeficientes, audio los lee
  void updateCoefficients();
};

} // namespace pg
