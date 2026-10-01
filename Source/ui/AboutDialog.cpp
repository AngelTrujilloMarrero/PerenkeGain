#include "ui/AboutDialog.h"
#include "BinaryData.h"
#include "types/Text.h"
#include "ui/RetroLookAndFeel.h"
#include "updater/AppVersion.h"

namespace pg {

AboutDialog::AboutDialog() {
  hero = juce::ImageCache::getFromMemory(BinaryData::logohero_jpeg,
                                         BinaryData::logohero_jpegSize);
}

void AboutDialog::paint(juce::Graphics &g) {
  g.setColour(retro::face());
  g.fillAll();

  auto r = getLocalBounds().reduced(10);
  auto text = r.removeFromBottom(78);

  if (hero.isValid()) {
    g.setColour(juce::Colours::black.withAlpha(0.12f));
    g.fillRoundedRectangle(r.toFloat().expanded(2.0f), 6.0f);
    g.drawImageWithin(hero, r.getX(), r.getY(), r.getWidth(), r.getHeight(),
                      juce::RectanglePlacement::centred);
  }

  g.setColour(juce::Colours::black);
  g.setFont(juce::Font(juce::FontOptions(18.0f, juce::Font::bold)));
  g.drawText("PerenkeGain", text.removeFromTop(26),
             juce::Justification::centred);
  g.setFont(juce::Font(juce::FontOptions(13.0f)));
  g.drawText(PG_T("Editor y Reproductor de Sonido"), text.removeFromTop(20),
             juce::Justification::centred);
  g.setFont(juce::Font(juce::FontOptions(12.0f)));
  g.drawText("Canary Audio  \xc2\xb7  v" + appVersion() +
                 "  \xc2\xb7  Licencia MIT",
             text, juce::Justification::centred);
}

} // namespace pg
