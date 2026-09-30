#include "ui/FileInfoBar.h"
#include "types/Text.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

juce::String FileInfoBar::timeStr(double sec) {
  int s = (int)sec;
  int cs = (int)((sec - s) * 100.0);
  return juce::String::formatted("%02d:%02d:%02d-%02d", s / 3600,
                                 (s / 60) % 60, s % 60, cs);
}

void FileInfoBar::setFile(const juce::String &n, double t) {
  fileName = n;
  total = t;
  repaint();
}

void FileInfoBar::setPosition(double p) {
  pos = p;
  repaint();
}

void FileInfoBar::setQuality(const juce::String &q) {
  quality = q;
  repaint();
}

void FileInfoBar::paint(juce::Graphics &g) {
  g.setColour(retro::face());
  g.fillAll();
  g.setColour(juce::Colours::black);
  g.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::plain)));
  auto top = getLocalBounds().removeFromTop(getHeight() / 2);
  g.drawText(fileName, top, juce::Justification::centredLeft, false);
  auto line1 = juce::String(PG_T("Duración total: ")) + timeStr(total) +
               PG_T("     Posición de reproducción: ") + timeStr(pos);
  auto line2 = juce::String(PG_T("Calidad de grabación: ")) + quality;
  auto bottom = getLocalBounds().withTrimmedTop(getHeight() / 2);
  g.drawText(line1, bottom, juce::Justification::centredLeft, false);
  g.drawText(line2, bottom, juce::Justification::centredRight, false);
}

} // namespace pg
