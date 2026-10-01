#pragma once
#include <JuceHeader.h>
#include "types/Text.h"

namespace pg {

// Version inyectada por CMake (PG_VERSION) para tener una sola fuente de
// verdad entre el binario, los paquetes y la comprobacion de actualizaciones.
inline juce::String appVersion() {
#ifdef PG_VERSION
  return PG_T(PG_VERSION);
#else
  return "0.0.0";
#endif
}

} // namespace pg
