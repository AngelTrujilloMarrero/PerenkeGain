#include "ui/AboutDialog.h"
#include "BinaryData.h"
#include "types/Text.h"
#include "ui/RetroLookAndFeel.h"
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
                 juce::Colour(0xFF1A5FB4));
}

void AboutDialog::resized() {
  auto r = getLocalBounds().reduced(12);
  auto text = r.removeFromBottom(134);
  text.removeFromTop(102); // deja hueco para las lineas de texto
  repo.setBounds(text.reduced(24, 4));
}

void AboutDialog::paint(juce::Graphics &g) {
  g.setColour(retro::face());
  g.fillAll();

  auto r = getLocalBounds().reduced(12);
  auto text = r.removeFromBottom(134).removeFromTop(102);

  if (hero.isValid())
    g.drawImageWithin(hero, r.getX(), r.getY(), r.getWidth(), r.getHeight(),
                      juce::RectanglePlacement::centred);

  g.setColour(juce::Colours::black);
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
