#include "audio/WaveformComponent.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

WaveformComponent::WaveformComponent() {
  formats.registerBasicFormats();
  thumb.addChangeListener(this);
  addAndMakeVisible(scrollbar);
}

void WaveformComponent::openFile(const juce::File &f) {
  clearSelection();
  thumb.setSource(new juce::FileInputSource(f));
  scrollbar.setRangeLimits(0.0, thumb.getTotalLength());
  scrollbar.setCurrentRange(0.0, thumb.getTotalLength() / 50.0);
}

void WaveformComponent::setPlayhead(double sec) {
  playheadSec = sec;
  scrollbar.setCurrentRangeStart(juce::jmax(0.0, sec));
  repaint();
}

void WaveformComponent::setMarkers(const std::vector<TrackRegion> &m) {
  markers = m;
  repaint();
}

void WaveformComponent::setVerticalZoom(float z) {
  verticalZoom = z;
  repaint();
}

void WaveformComponent::changeListenerCallback(juce::ChangeBroadcaster *) {
  scrollbar.setRangeLimits(0.0, thumb.getTotalLength());
  repaint();
}

juce::Rectangle<int> WaveformComponent::waveArea() {
  auto r = getLocalBounds().reduced(2);
  scrollbar.setBounds(r.removeFromBottom(16));
  r.removeFromBottom(2);
  return r;
}

juce::Rectangle<int> WaveformComponent::innerArea() const {
  auto r = getLocalBounds().reduced(2);
  r.removeFromBottom(16);
  r.removeFromBottom(2);
  return r.reduced(2);
}

double WaveformComponent::xToSec(double x) const {
  auto inner = innerArea();
  double t = (x - inner.getX()) / juce::jmax(1, inner.getWidth());
  return juce::jlimit(0.0, thumb.getTotalLength(), t * thumb.getTotalLength());
}

void WaveformComponent::mouseDown(const juce::MouseEvent &e) {
  if (thumb.getTotalLength() <= 0.0 ||
      !innerArea().contains(e.position.toInt()))
    return;
  selA = selB = xToSec(e.position.x);
  repaint();
}

void WaveformComponent::mouseDrag(const juce::MouseEvent &e) {
  if (selA < 0.0)
    return;
  selB = xToSec(e.position.x);
  repaint();
}

void WaveformComponent::mouseUp(const juce::MouseEvent &e) {
  if (selA < 0.0)
    return;
  auto inner = innerArea();
  double px = thumb.getTotalLength() / juce::jmax(1, inner.getWidth());
  if (juce::jmax(selA, selB) - juce::jmin(selA, selB) < 2.0 * px) {
    clearSelection(); // clic simple: no crea selección
    if (onSeek)
      onSeek(xToSec(e.position.x)); // ... pero salta a esa posición
    return;
  }
  if (onSelectionChanged) {
    auto s = getSelection();
    onSelectionChanged(s.first, s.second);
  }
}

std::pair<double, double> WaveformComponent::getSelection() const {
  return {juce::jmin(selA, selB), juce::jmax(selA, selB)};
}

void WaveformComponent::clearSelection() {
  selA = selB = -1.0;
  repaint();
}

void WaveformComponent::paintSelection(juce::Graphics &g,
                                       const juce::Rectangle<int> &inner) {
  if (!hasSelection() || thumb.getTotalLength() <= 0.0)
    return;
  double total = thumb.getTotalLength();
  auto [a, b] = getSelection();
  int x1 = inner.getX() + int(a / total * inner.getWidth());
  int x2 = inner.getX() + int(b / total * inner.getWidth());
  auto sel = juce::Rectangle<int>(x1, inner.getY(), juce::jmax(1, x2 - x1),
                                  inner.getHeight())
                 .getIntersection(inner);
  g.setColour(juce::Colour(0x663355CC)); // azul translúcido estilo clásico
  g.fillRect(sel);
  g.setColour(juce::Colour(0xCC3355CC));
  g.drawVerticalLine(sel.getX(), (float)inner.getY(),
                     (float)inner.getBottom());
  g.drawVerticalLine(sel.getRight(), (float)inner.getY(),
                     (float)inner.getBottom());
}

void WaveformComponent::paint(juce::Graphics &g) {
  auto r = waveArea();
  g.setColour(retro::face());
  g.fillRect(r);
  retro::bevelSunken(g, r);

  auto inner = r.reduced(2);
  g.setColour(juce::Colours::white);
  g.fillRect(inner);

  if (thumb.getNumChannels() > 0) {
    g.saveState();
    g.reduceClipRegion(inner);
    g.setColour(juce::Colours::black);
    thumb.drawChannels(g, inner, 0.0, thumb.getTotalLength(), verticalZoom);
    paintSelection(g, inner);
    for (auto &m : markers) {
      double total = juce::jmax(1e-9, thumb.getTotalLength());
      g.setColour(juce::Colours::red);
      int x1 = inner.getX() + int(m.startSec / total * inner.getWidth());
      g.drawVerticalLine(x1, (float)inner.getY(), (float)inner.getBottom());
      if (m.endSec > m.startSec) { // extremo del tramo de corte
        int x2 = inner.getX() + int(m.endSec / total * inner.getWidth());
        g.drawVerticalLine(x2, (float)inner.getY(), (float)inner.getBottom());
      }
    }
    if (playheadSec >= 0.0) {
      int x = inner.getX() + int(playheadSec / thumb.getTotalLength() *
                                 inner.getWidth());
      g.setColour(juce::Colour(0xFFFF0000));
      g.drawVerticalLine(x, (float)inner.getY(), (float)inner.getBottom());
    }
    g.restoreState();
  } else {
    g.setColour(juce::Colour(0xFF808080));
    g.drawFittedText("Abrir un archivo WAV / MP3 / FLAC...", inner,
                     juce::Justification::centred, 1);
  }
}

} // namespace pg
