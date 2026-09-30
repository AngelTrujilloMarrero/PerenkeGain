#pragma once
#include <array>

namespace pg {

// 31 bandas ISO 1/3 octava 20 Hz–20 kHz.
inline constexpr std::array<float, 31> kEq31Freqs = {
  20.f, 25.f, 31.5f, 40.f, 50.f, 63.f, 80.f, 100.f, 125.f, 160.f, 200.f,
  250.f, 315.f, 400.f, 500.f, 630.f, 800.f, 1000.f, 1250.f, 1600.f, 2000.f,
  2500.f, 3150.f, 4000.f, 5000.f, 6300.f, 8000.f, 10000.f, 12500.f, 16000.f,
  20000.f
};

struct Eq31Band {
  float gainDb = 0.0f;      // -12..+12
  float intensity = 1.0f;   // 0..1 (nivel integrado por banda)
  bool enabled = true;
};

struct Eq31State {
  std::array<Eq31Band, 31> bands{};
  bool bypass = false;
  float masterIntensity = 1.0f; // 0..1
};

} // namespace pg
