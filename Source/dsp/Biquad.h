#pragma once
#include <cmath>

namespace pg {

// Biquad minimo y rapido (transposed direct form II), todo inline.
// Sustituye a juce::dsp::IIR::Filter, cuya indireccion por puntero a los
// coeficientes lo hace ~20x mas lento en el hilo de audio.
struct Biquad {
  float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
  float z1 = 0.0f, z2 = 0.0f;

  void reset() noexcept { z1 = z2 = 0.0f; }

  inline float process(float x) noexcept {
    const float y = b0 * x + z1;
    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;
    return y;
  }

  // Filtro peak RBJ (igual que makePeakFilter de JUCE).
  void setPeak(double sr, double freq, double q, double gainFactor) noexcept {
    const double kPi = 3.14159265358979323846;
    const double A = std::sqrt(gainFactor);
    const double w = 2.0 * kPi * freq / sr;
    const double cw = std::cos(w);
    const double alpha = std::sin(w) / (2.0 * q);
    const double a0 = 1.0 + alpha / A;
    b0 = (float)((1.0 + alpha * A) / a0);
    b1 = (float)((-2.0 * cw) / a0);
    b2 = (float)((1.0 - alpha * A) / a0);
    a1 = (float)((-2.0 * cw) / a0);
    a2 = (float)((1.0 - alpha / A) / a0);
  }

  // Filtro band-pass RBJ (igual que makeBandPass de JUCE).
  void setBandPass(double sr, double freq, double q) noexcept {
    const double kPi = 3.14159265358979323846;
    const double w = 2.0 * kPi * freq / sr;
    const double cw = std::cos(w);
    const double alpha = std::sin(w) / (2.0 * q);
    const double a0 = 1.0 + alpha;
    b0 = (float)(alpha / a0);
    b1 = 0.0f;
    b2 = (float)(-alpha / a0);
    a1 = (float)((-2.0 * cw) / a0);
    a2 = (float)((1.0 - alpha) / a0);
  }
};

} // namespace pg
