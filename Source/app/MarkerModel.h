#pragma once
#include <JuceHeader.h>
#include <vector>
#include "types/AudioTypes.h"

namespace pg {

// Marcadores/cortes de la pista actual. Fuente unica compartida por la ventana
// de onda (edicion) y la exportacion por tramos.
class MarkerModel : public juce::ChangeBroadcaster {
public:
  void setFile(const juce::File &f);
  const juce::File &file() const { return current; }

  void clear();
  void addMarker(double a, double b);
  void setMarkers(std::vector<TrackRegion> m);
  const std::vector<TrackRegion> &markers() const { return cuts; }

  // Convierte puntos de corte (segundos) en tramos consecutivos no solapados.
  static std::vector<TrackRegion> segmentsFromCuts(std::vector<double> cuts,
                                                   double total);

private:
  juce::File current;
  std::vector<TrackRegion> cuts;
};

} // namespace pg
