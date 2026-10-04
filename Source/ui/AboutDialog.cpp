#include "ui/AboutDialog.h"
#include "BinaryData.h"
#include "types/Text.h"
#include "ui/ModernLookAndFeel.h"
#include "updater/AppVersion.h"

namespace pg {

AboutDialog::AboutDialog()
    : repo("GitHub: AngelTrujilloMarrero/PerenkeGain",
           juce::URL(
               "https://github.com/AngelTrujilloMarrero/PerenkeGain")) {
  hero = juce::ImageCache::getFromMemory(BinaryData::logohero_jpeg,
                                         BinaryData::logohero_jpegSize);
  addAndMakeVisible(repo);
  repo.setFont(juce::Font(juce::FontOptions(12.0f)), false);
  repo.setColour(juce::HyperlinkButton::textColourId,
                 juce::Colour(0xFF4FA3FF));

  addAndMakeVisible(closeB);
  closeB.onClick = [this] {
    // En Android la ventana no tiene barra de titulo con boton de cierre.
    if (auto *dw = findParentComponentOfClass<juce::DialogWindow>())
      dw->exitModalState(0);
  };
}

juce::Rectangle<int> AboutDialog::textBounds() const {
  auto r = getLocalBounds().reduced(12).removeFromBottom(168);
  r.removeFromBottom(34); // boton Cerrar
  r.removeFromBottom(6);
  r.removeFromBottom(28); // enlace GitHub
  return r;
}

void AboutDialog::resized() {
  auto bottom = getLocalBounds().reduced(12).removeFromBottom(168);
  closeB.setBounds(bottom.removeFromBottom(34).withSizeKeepingCentre(140, 28));
  bottom.removeFromBottom(6);
  repo.setBounds(bottom.removeFromBottom(28).reduced(24, 2));
}

void AboutDialog::paint(juce::Graphics &g) {
  g.setColour(modern::surface());
  g.fillAll();

  auto r = getLocalBounds().reduced(12);
  r.removeFromBottom(168);
  if (hero.isValid())
    g.drawImageWithin(hero, r.getX(), r.getY(), r.getWidth(), r.getHeight(),
                      juce::RectanglePlacement::centred);

  auto text = textBounds();
  g.setColour(juce::Colour(0xFFF2F3F6));
  g.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
  g.drawText("PerenkeGain", text.removeFromTop(26),
             juce::Justification::centred);
  g.setFont(juce::Font(juce::FontOptions(13.0f)));
  g.drawText(PG_T("Editor y Reproductor de Sonido"), text.removeFromTop(20),
             juce::Justification::centred);
  g.setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
  g.drawText(PG_T("\u00c1ngel Trujillo Marrero"), text.removeFromTop(20),
             juce::Justification::centred);
  g.setFont(juce::Font(juce::FontOptions(12.0f)));
  g.drawText("atrujimar@gmail.com", text.removeFromTop(18),
             juce::Justification::centred);
  g.drawText("Licencia MIT  \xc2\xb7  v" + appVersion(), text,
             juce::Justification::centred);
}

} // namespace pg
