#pragma once
#include <array>
#include "types/EqTypes.h"
#include "dsp/EqPresetsLatin.h"

namespace pg {

// Ecualizaciones preconfiguradas.
//
// Van compiladas en el binario ("en cache"): aplicarlas es instantaneo,
// sin descargas ni calculos en tiempo de ejecucion. Cada preset guarda
// la ganancia en dB (-12..+12, pasos de 0.5) para las 31 bandas ISO de
// kEq31Freqs (20 Hz..20 kHz).
//
// kEqPresetsBase: generos occidentales + utilitarios.
// kEqPresetsLatin: generos latinos (ver EqPresetsLatin.h).
// kEqPresets: lista completa que consume la UI.
inline constexpr std::array<EqPreset, 10> kEqPresetsBase = {{
    {"Plano",
     {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},
    {"Rock",
     {{4.5, 4, 3.5, 3, 3, 2.5, 2, 1.5, 1, 0.5, 0, -0.5, -1, -1, -1, -0.5,
       0, 0.5, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 3.5, 4, 4, 3.5, 3, 2.5}}},
    {"Pop",
     {{3, 2.5, 2, 2, 1.5, 1.5, 1, 1, 0.5, 0.5, 0, 0, 0, 0, 0.5, 0.5,
       1, 1, 1, 1.5, 1.5, 2, 2, 2, 2.5, 2.5, 3, 3, 2.5, 2, 1.5}}},
    {"Jazz",
     {{3.5, 3, 2.5, 2.5, 2, 2, 1.5, 1.5, 1, 1, 0.5, 0.5, 0, 0, 0, 0,
       0.5, 0.5, 0.5, 1, 1, 1, 1.5, 1.5, 2, 2, 2, 1.5, 1, 0.5, 0}}},
    {"Cl\u00e1sica",
     {{2.5, 2, 2, 1.5, 1.5, 1, 1, 0.5, 0.5, 0, 0, 0, 0, 0, -0.5, -0.5,
       0, 0, 0, 0.5, 0.5, 1, 1, 1.5, 1.5, 2, 2, 2, 1.5, 1, 0.5}}},
    {"Electr\u00f3nica",
     {{5, 4.5, 4.5, 4, 4, 3.5, 3, 2.5, 2, 1.5, 1, 0.5, 0, 0, 0, 0,
       0.5, 0.5, 1, 1, 1.5, 1.5, 2, 2.5, 3, 3.5, 4, 4, 3.5, 3, 2.5}}},
    {"Hip-Hop",
     {{5.5, 5, 5, 4.5, 4.5, 4, 3.5, 3, 2.5, 2, 1.5, 1, 0.5, 0.5, 0, 0,
       0, 0.5, 0.5, 0.5, 1, 1, 1, 1.5, 1.5, 2, 2, 2, 1.5, 1, 0.5}}},
    {"Vocal",
     {{-4, -3, -2, -1.5, -1, -1, -0.5, -0.5, 0, 0, 0, 0, 0.5, 0.5, 1, 1,
       1.5, 1.5, 2, 2.5, 2.5, 3, 3, 2.5, 2, 1.5, 1, 0.5, 0, 0, -0.5}}},
    {"Graves potentes",
     {{6, 5.5, 5, 4.5, 4, 3.5, 3, 2.5, 2, 1.5, 1, 0.5, 0.5, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}},
    {"Brillo",
     {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5,
       0.5, 1, 1, 1.5, 1.5, 2, 2, 2.5, 2.5, 3, 3.5, 3.5, 3, 2.5, 2}}},
}};

// Lista completa que ve la UI: presets base + generos latinos.
inline constexpr auto kEqPresets = [] {
  std::array<EqPreset, kEqPresetsBase.size() + kEqPresetsLatin.size()> all{};
  size_t n = 0;
  for (const auto &p : kEqPresetsBase)
    all[n++] = p;
  for (const auto &p : kEqPresetsLatin)
    all[n++] = p;
  return all;
}();

// Convierte un preset en estado aplicable al EQ (bandas activas,
// intensidad al 100 %; el master se conserva aparte).
inline Eq31State eqPresetState(const EqPreset &p) {
  Eq31State s{};
  for (size_t i = 0; i < 31; ++i) {
    s.bands[i].gainDb = p.gains[i];
    s.bands[i].intensity = 1.0f;
    s.bands[i].enabled = true;
  }
  return s;
}

} // namespace pg
