#pragma once
#include <JuceHeader.h>
#include "types/AudioTypes.h"

namespace pg {

// Waveform estilo PolderbitS: marco hundido, fondo blanco, onda negra,
// scrollbar horizontal, playhead rojo, marcadores rojos y selección de
// tramo con arrastre del ratón (para marcar cortes sobre la onda).
class WaveformComponent : public juce::Component,
                          public juce::ChangeListener {
public:
  WaveformComponent();
  void paint(juce::Graphics &g) override;
  void changeListenerCallback(juce::ChangeBroadcaster *) override;
  void openFile(const juce::File &f);
  void setPlayhead(double sec);
  void setMarkers(const std::vector<TrackRegion> &m);
  void setVerticalZoom(float z);

  void mouseDown(const juce::MouseEvent &e) override;
  void mouseDrag(const juce::MouseEvent &e) override;
  void mouseUp(const juce::MouseEvent &e) override;

  // Selección de tramo en segundos (siempre de menor a mayor).
  bool hasSelection() const { return selA >= 0.0; }
  std::pair<double, double> getSelection() const;
  void clearSelection();
  // Se dispara al soltar el ratón con un tramo seleccionado.
  std::function<void(double, double)> onSelectionChanged;
  // Se dispara con un clic simple (sin arrastre): salta a esa posición.
  std::function<void(double)> onSeek;

private:
  juce::AudioFormatManager formats;
  juce::AudioThumbnailCache cache{5};
  juce::AudioThumbnail thumb{512, formats, cache};
  juce::ScrollBar scrollbar{false};
  double playheadSec = -1.0;
  float verticalZoom = 1.0f;
  std::vector<TrackRegion> markers;
  double selA = -1.0, selB = -1.0;

  juce::Rectangle<int> waveArea();
  juce::Rectangle<int> innerArea() const;
  double xToSec(double x) const;
  void paintSelection(juce::Graphics &g, const juce::Rectangle<int> &inner);
};

} // namespace pg
