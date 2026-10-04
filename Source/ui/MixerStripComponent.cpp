#include "ui/MixerStripComponent.h"

namespace pg {

static constexpr float kGainMin = -12.0f, kGainMax = 12.0f;
static constexpr int kMuteH = 16;

MixerStripComponent::MixerStripComponent(int bandIndex) : band(bandIndex) {
  setOpaque(true);
}

juce::Rectangle<int> MixerStripComponent::meterArea() const {
  return {0, 0, juce::jmin(8, getWidth()),
          juce::jmax(0, getHeight() - kMuteH - 2)};
}

juce::Rectangle<int> MixerStripComponent::faderArea() const {
  int x = juce::jmin(9, getWidth());
  int w = juce::jmax(0, getWidth() - x - 3);
  int h = juce::jmax(0, getHeight() - kMuteH - 6);
  return {x, 2, w, h};
}

juce::Rectangle<int> MixerStripComponent::muteArea() const {
  return {2, getHeight() - kMuteH, juce::jmax(0, getWidth() - 4), kMuteH};
}

void MixerStripComponent::setLevelDb(float db) {
  if (std::abs(db - level) < 0.5f)
    return;
  level = db;
  repaint(meterArea().expanded(1, 0));
}

void MixerStripComponent::setGainDb(float db) {
  gain = juce::jlimit(kGainMin, kGainMax, db);
  repaint(faderArea().expanded(1, 1));
}

void MixerStripComponent::setMuted(bool m) {
  muted = m;
  repaint(muteArea().expanded(1, 1));
}

void MixerStripComponent::mouseDown(const juce::MouseEvent &e) {
  if (muteArea().contains(e.position.toInt())) {
    muted = !muted;
    repaint(muteArea().expanded(1, 1));
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
  float travel = (float)juce::jmax(1, faderArea().getHeight() - 6);
  float perPx = (kGainMax - kGainMin) / travel;
  float g = dragStartGain + (dragStartY - (float)e.position.y) * perPx;
  g = juce::jlimit(kGainMin, kGainMax, g);
  g = std::round(g * 2.0f) / 2.0f; // pasos de 0.5 dB
  if (std::abs(g - gain) > 0.01f) {
    gain = g;
    repaint(faderArea().expanded(1, 1));
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

// Color del capuchón según la banda: espectro graves(azul)->agudos(rojo),
// como los faders de colores de una mesa de sonido real.
juce::Colour MixerStripComponent::capColour() const {
  float t = juce::jlimit(0.0f, 1.0f, (float)band / 30.0f);
  float hue = juce::jmap(t, 0.0f, 1.0f, 0.63f, 0.0f);
  return juce::Colour::fromHSV(hue, 0.62f, 0.82f, 1.0f);
}

void MixerStripComponent::paintFader(juce::Graphics &g) {
  auto r = faderArea();
  if (r.getWidth() < 6 || r.getHeight() < 24)
    return;
  float cx = (float)r.getCentreX();

  // Carril hundido con gradiente (aspecto consola).
  auto slot = r.toFloat().reduced(1.0f, 3.0f);
  juce::ColourGradient slotGrad(juce::Colour(0xFF050608), slot.getCentreX(),
                                slot.getY(), juce::Colour(0xFF181B22),
                                slot.getCentreX(), slot.getBottom(), false);
  g.setGradientFill(slotGrad);
  g.fillRoundedRectangle(slot, 3.0f);
  g.setColour(juce::Colour(0xFF3A3E48));
  g.drawRoundedRectangle(slot.reduced(0.5f), 3.0f, 1.0f);

  // Riel central marcado.
  g.setColour(juce::Colour(0xFF000000));
  g.fillRect(cx - 1.5f, slot.getY() + 2.0f, 3.0f, slot.getHeight() - 4.0f);
  g.setColour(juce::Colour(0xFF2C3038));
  g.fillRect(cx - 0.5f, slot.getY() + 2.0f, 1.0f, slot.getHeight() - 4.0f);

  // Ticks de escala (-12, -6, 0, +6, +12 dB).
  constexpr int ticks = 4;
  g.setColour(juce::Colour(0xFF6A7080));
  for (int i = 0; i <= ticks; ++i) {
    float frac = (float)i / (float)ticks;
    int y = juce::roundToInt(slot.getY() + 3.0f +
                             (1.0f - frac) * (slot.getHeight() - 6.0f));
    bool mid = (i == ticks / 2);
    float len = mid ? slot.getWidth() * 0.55f : 3.0f;
    g.drawHorizontalLine(y, slot.getX() + 1.0f, slot.getX() + 1.0f + len);
  }

  // Capuchón grande tipo fader de consola, con color propio por banda.
  float frac = (gain - kGainMin) / (kGainMax - kGainMin);
  float capH = juce::jlimit(22.0f, 30.0f, (float)r.getHeight() / 6.5f);
  float capCentreY =
      slot.getY() + 3.0f + (1.0f - frac) * (slot.getHeight() - 6.0f);
  float capW = (float)r.getWidth();
  auto cap = juce::Rectangle<float>(cx - capW * 0.5f, capCentreY - capH * 0.5f,
                                    capW, capH);
  juce::Colour col = capColour();

  // Sombra proyectada.
  g.setColour(juce::Colour(0xA0000000));
  g.fillRoundedRectangle(cap.translated(0.0f, 2.5f), 4.0f);

  // Cuerpo con gradiente vertical coloreado (arriba claro, abajo oscuro).
  juce::ColourGradient body(col.brighter(0.60f), cap.getCentreX(), cap.getY(),
                            col.darker(0.55f), cap.getCentreX(), cap.getBottom(),
                            false);
  body.addColour(0.50, col.withMultipliedBrightness(1.05f));
  g.setGradientFill(body);
  g.fillRoundedRectangle(cap, 4.0f);

  // Bisel: borde oscuro que da volumen.
  g.setColour(col.darker(0.80f));
  g.drawRoundedRectangle(cap.reduced(0.5f), 4.0f, 1.4f);

  // Rebaje cóncavo central (donde apoya el dedo).
  auto groove = cap.reduced(capW * 0.16f, capH * 0.30f);
  g.setColour(juce::Colour(0x55000000));
  g.fillRoundedRectangle(groove.translated(0.0f, 1.0f), 2.0f);
  g.setColour(juce::Colour(0x40FFFFFF));
  g.drawRoundedRectangle(groove, 2.0f, 1.0f);

  // Línea indicadora central contrastada.
  g.setColour(col.contrasting(0.9f));
  g.fillRect(cap.getX() + 1.5f, cap.getCentreY() - 0.75f, cap.getWidth() - 3.0f,
             1.5f);

  // Reflejo superior brillante.
  g.setColour(juce::Colour(0xA0FFFFFF));
  g.drawHorizontalLine(juce::roundToInt(cap.getY() + 1.5f), cap.getX() + 3.0f,
                       cap.getRight() - 3.0f);
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
  auto b = getLocalBounds();
  juce::ColourGradient bg(juce::Colour(0xFF2C2F3A), 0.0f, 0.0f,
                          juce::Colour(0xFF1B1D24), 0.0f, (float)b.getHeight(),
                          false);
  g.setGradientFill(bg);
  g.fillRect(b);
  // Separación entre canales (efecto consola).
  g.setColour(juce::Colour(0xFF0E0F12));
  g.drawVerticalLine(0, 0.0f, (float)b.getHeight());
  g.drawVerticalLine(b.getRight() - 1, 0.0f, (float)b.getHeight());
  paintMeter(g);
  paintFader(g);
  paintMute(g);
}

} // namespace pg
