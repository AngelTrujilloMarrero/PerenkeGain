#pragma once

namespace pg {

// Configuracion del nivelador dinamico en vivo (solo monitorizacion durante
// la reproduccion; no toca la exportacion).
struct LevelerParams {
  bool enabled = false;
  float targetLufs = -14.0f; // objetivo de sonoridad percibida
  float maxBoostDb = 12.0f;  // subida maxima permitida
  float maxCutDb = 24.0f;    // bajada maxima permitida
  float attackSec = 0.2f;    // reaccion al bajar ganancia (rapida)
  float releaseSec = 1.5f;   // reaccion al subir ganancia (lenta)
  float ceilingDb = -1.0f;   // techo true-peak de seguridad
  float gateLufs = -60.0f;   // por debajo no se sube (silencio/ruido)
};

// Mediciones expuestas a la UI (escritas en el hilo de audio).
struct LevelerMeters {
  float momentaryLufs = -70.0f;
  float shortTermLufs = -70.0f;
  float appliedGainDb = 0.0f;
  float peakDb = -70.0f;
};

} // namespace pg
