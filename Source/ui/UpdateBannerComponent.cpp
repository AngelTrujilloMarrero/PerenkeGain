#include "ui/UpdateBannerComponent.h"
#include "types/Text.h"

namespace pg {

namespace {
const juce::Colour kTop(0xFF0B3C5D);
const juce::Colour kBottom(0xFF17A2B8);

juce::Path makeCrossPath() {
  juce::Path p;
  p.startNewSubPath(0.28f, 0.28f);
  p.lineTo(0.72f, 0.72f);
  p.startNewSubPath(0.72f, 0.28f);
  p.lineTo(0.28f, 0.72f);
  return p;
}
} // namespace

UpdateBannerComponent::UpdateBannerComponent()
    : closeBtn("cerrar", juce::Colours::white.withAlpha(0.75f),
               juce::Colours::white, juce::Colours::white.withAlpha(0.5f)) {
  closeBtn.setShape(makeCrossPath(), true, true, false);
  closeBtn.setMouseCursor(juce::MouseCursor::PointingHandCursor);
  closeBtn.onClick = [this] {
    if (onDismiss)
      onDismiss();
  };
  addAndMakeVisible(closeBtn);

  setMouseCursor(juce::MouseCursor::PointingHandCursor);
  setInterceptsMouseClicks(true, true);
}

void UpdateBannerComponent::setVersion(const juce::String &latest,
                                       const juce::String &current) {
  latestVersion = latest;
  currentVersion = current;
  repaint();
}

void UpdateBannerComponent::mouseEnter(const juce::MouseEvent &) {
  hover = true;
  repaint();
}

void UpdateBannerComponent::mouseExit(const juce::MouseEvent &) {
  hover = false;
  repaint();
}

void UpdateBannerComponent::mouseUp(const juce::MouseEvent &) {
  if (onShowDetails)
    onShowDetails();
}

void UpdateBannerComponent::paint(juce::Graphics &g) {
  auto r = getLocalBounds().toFloat();
  juce::ColourGradient grad(kTop, r.getTopLeft(), kBottom, r.getBottomRight(),
                            false);
  g.setGradientFill(grad);
  g.fillRoundedRectangle(r, 5.0f);

  if (hover) {
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.fillRoundedRectangle(r, 5.0f);
  }

  // Icono circular con flecha de actualizacion.
  auto icon = r.removeFromLeft(r.getHeight()).reduced(7.0f);
  g.setColour(juce::Colours::white.withAlpha(0.95f));
  g.drawEllipse(icon, 1.6f);
  juce::Path arrow;
  arrow.startNewSubPath(icon.getCentreX(), icon.getBottom() - 4.0f);
  arrow.lineTo(icon.getCentreX(), icon.getY() + 4.0f);
  arrow.startNewSubPath(icon.getCentreX() - 3.5f, icon.getY() + 7.0f);
  arrow.lineTo(icon.getCentreX(), icon.getY() + 3.5f);
  arrow.lineTo(icon.getCentreX() + 3.5f, icon.getY() + 7.0f);
  g.strokePath(arrow, juce::PathStrokeType(1.6f));

  auto text = r.reduced(6.0f, 0.0f);
  auto title = text.removeFromTop(text.getHeight() * 0.56f);
  g.setColour(juce::Colours::white);
  g.setFont(juce::Font(juce::FontOptions(13.5f, juce::Font::bold)));
  g.drawText(PG_T("Nueva versi\u00f3n disponible: v") + latestVersion, title,
             juce::Justification::centredLeft);

  g.setColour(juce::Colours::white.withAlpha(0.82f));
  g.setFont(juce::Font(juce::FontOptions(11.5f)));
  g.drawText(PG_T("Tienes la v") + currentVersion +
                 PG_T(". Pulsa para ver las novedades e instalar."),
             text, juce::Justification::centredLeft);
}

void UpdateBannerComponent::resized() {
  auto r = getLocalBounds();
  closeBtn.setBounds(r.removeFromRight(30).withSizeKeepingCentre(14, 14));
}

} // namespace pg
