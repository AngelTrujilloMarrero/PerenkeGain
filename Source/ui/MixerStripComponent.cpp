#include "ui/MixerStripComponent.h"

namespace pg {

static constexpr float kGainMin = -12.0f, kGainMax = 12.0f;

MixerStripComponent::MixerStripComponent(int bandIndex) : band(bandIndex) {}

juce::Rectangle<int> MixerStripComponent::meterArea() const {
  return {0, 0, juce::jmin(8, getWidth()), getHeight()};
}

juce::Rectangle<int> MixerStripComponent::faderArea() const {
  return {9, 0, 11, getHeight()};
}

juce::Rectangle<int> MixerStripComponent::muteArea() const {
  int h = juce::jmin(15, getHeight());
  return {20, getHeight() - h, juce::jmax(0, getWidth() - 20), h};
}

void MixerStripComponent::setLevelDb(float db) {
  if (std::abs(db - level) < 0.5f)
    return;
  level = db;
  repaint();
}

void MixerStripComponent::setGainDb(float db) {
  gain = juce::jlimit(kGainMin, kGainMax, db);
  repaint();
}

void MixerStripComponent::setMuted(bool m) {
  muted = m;
  repaint();
}

void MixerStripComponent::mouseDown(const juce::MouseEvent &e) {
  if (muteArea().contains(e.position.toInt())) {
    muted = !muted;
    repaint();
    if (onEdit)
      onEdit();
    return;
  }
  if (faderArea().contains(e.position.toInt())) {
    dragging = true;
    dragStartY = e.position.y;
    dragStartGain = gain;
  }
}

void MixerStripComponent::mouseDrag(const juce::MouseEvent &e) {
  if (!dragging)
    return;
  float perPx = (kGainMax - kGainMin) / (float)juce::jmax(1, getHeight());
  float g = dragStartGain + (dragStartY - (float)e.position.y) * perPx;
  g = juce::jlimit(kGainMin, kGainMax, g);
  g = std::round(g * 2.0f) / 2.0f; // pasos de 0.5 dB
  if (std::abs(g - gain) > 0.01f) {
    gain = g;
    repaint();
    if (onEdit)
      onEdit();
  }
}

void MixerStripComponent::mouseUp(const juce::MouseEvent &) {
  dragging = false;
}

void MixerStripComponent::paintMeter(juce::Graphics &g) {
  auto r = meterArea();
  if (r.getWidth() <= 0)
    return;
  g.setColour(juce::Colour(0xFF0B0C0F));
  g.fillRect(r);
  g.setColour(juce::Colours::black);
  g.drawRect(r, 1);
  auto inner = r.reduced(1);
  constexpr int segs = 16;
  float t = juce::jlimit(0.0f, 1.0f, (level + 60.0f) / 60.0f);
  int lit = juce::roundToInt(t * segs);
  int segH = juce::jmax(2, inner.getHeight() / segs);
  for (int i = 0; i < segs; ++i) {
    auto seg = juce::Rectangle<int>(inner.getX(),
                                    inner.getBottom() - (i + 1) * segH - i,
                                    inner.getWidth(), segH);
    if (seg.getY() < inner.getY())
      break;
    if (i >= lit) {
      g.setColour(juce::Colour(0xFF1A1D24)); // apagado
    } else if (i >= segs - 3) {
      g.setColour(juce::Colour(0xFFE2372A)); // rojo
    } else if (i >= segs - 6) {
      g.setColour(juce::Colour(0xFFE8B21C)); // ámbar
    } else {
      g.setColour(juce::Colour(0xFF21C43E)); // verde
    }
    g.fillRect(seg);
  }
}

void MixerStripComponent::paintFader(juce::Graphics &g) {
  auto r = faderArea();
  if (r.getWidth() <= 0 || r.getHeight() < 8)
    return;
  int cx = r.getCentreX();
  // Ranura
  g.setColour(juce::Colour(0xFF0A0B0E));
  g.fillRect(cx - 1, r.getY() + 2, 3, r.getHeight() - 4);
  g.setColour(juce::Colour(0xFF3A3E4A));
  g.drawVerticalLine(cx + 1, (float)(r.getY() + 2),
                     (float)(r.getBottom() - 2));
  // Capuchón que sube y baja
  constexpr int capH = 13;
  float frac = (gain - kGainMin) / (kGainMax - kGainMin);
  int capY = r.getY() + 3 +
             juce::roundToInt((1.0f - frac) * (float)(r.getHeight() - 6 - capH));
  juce::Rectangle<int> cap(cx - 4, capY, 9, capH);
  g.setColour(juce::Colour(0xFFE6E6E6));
  g.fillRoundedRectangle(cap.toFloat(), 2.0f);
  g.setColour(juce::Colours::black);
  g.drawRoundedRectangle(cap.toFloat(), 2.0f, 1.0f);
  g.setColour(juce::Colour(0xFF606470));
  g.drawHorizontalLine(cap.getCentreY(), cap.getX() + 2.0f,
                       cap.getRight() - 2.0f);
}

void MixerStripComponent::paintMute(juce::Graphics &g) {
  auto r = muteArea();
  if (r.getWidth() < 4 || r.getHeight() < 6)
    return;
  g.setColour(muted ? juce::Colour(0xFFC8281E) : juce::Colour(0xFF33363F));
  g.fillRoundedRectangle(r.toFloat(), 2.0f);
  g.setColour(juce::Colours::black);
  g.drawRoundedRectangle(r.toFloat(), 2.0f, 1.0f);
  g.setColour(muted ? juce::Colours::white : juce::Colour(0xFF7C8290));
  g.setFont(juce::Font(juce::FontOptions(8.0f, juce::Font::bold)));
  g.drawText("M", r, juce::Justification::centred);
}

void MixerStripComponent::paint(juce::Graphics &g) {
  g.setColour(juce::Colour(0xFF262933));
  g.fillRect(getLocalBounds());
  paintMeter(g);
  paintFader(g);
  paintMute(g);
}

} // namespace pg
