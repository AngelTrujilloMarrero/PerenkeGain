#include "ui/TransportButton.h"

namespace pg {

TransportButton::TransportButton(Icon i) : juce::Button(""), icon(i) {
  setSize(40, 40);
}

void TransportButton::setIcon(Icon i) {
  if (icon == i)
    return;
  icon = i;
  repaint();
}

void TransportButton::paintButton(juce::Graphics &g,
                                  bool shouldDrawButtonAsHighlighted,
                                  bool shouldDrawButtonAsDown) {
  auto b = getLocalBounds().toFloat();
  const float d = juce::jmin(b.getWidth(), b.getHeight()) - 4.0f;
  juce::Rectangle<float> circle(b.getCentreX() - d / 2.0f,
                                b.getCentreY() - d / 2.0f, d, d);

  juce::Colour bg = juce::Colour(0xFF262B36);
  juce::Colour iconCol = juce::Colour(0xFFE9EDF4);
  if (shouldDrawButtonAsDown)
    bg = juce::Colour(0xFF3C4658);
  else if (shouldDrawButtonAsHighlighted)
    bg = juce::Colour(0xFF323949);
  if (!isEnabled()) {
    bg = bg.withAlpha(0.4f);
    iconCol = iconCol.withAlpha(0.4f);
  }

  // Sombra suave y fondo circular.
  g.setColour(juce::Colours::black.withAlpha(0.35f));
  g.fillEllipse(circle.translated(0.0f, 1.5f));
  g.setColour(bg);
  g.fillEllipse(circle);
  g.setColour(juce::Colour(0xFF454E60));
  g.drawEllipse(circle, 1.0f);

  // Icono.
  g.setColour(iconCol);
  auto c = circle.reduced(circle.getWidth() * 0.30f);
  if (icon == Icon::Play) {
    juce::Path p;
    p.addTriangle(c.getX(), c.getY(), c.getX(), c.getBottom(), c.getRight(),
                  c.getCentreY());
    g.fillPath(p);
  } else if (icon == Icon::Pause) {
    const float w = c.getWidth() * 0.34f;
    g.fillRoundedRectangle(c.getX(), c.getY(), w, c.getHeight(), 1.5f);
    g.fillRoundedRectangle(c.getRight() - w, c.getY(), w, c.getHeight(), 1.5f);
  } else { // Stop
    g.fillRoundedRectangle(c, 2.5f);
  }
}

} // namespace pg
