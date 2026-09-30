#include "ui/EqualizerComponent.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

EqualizerComponent::EqualizerComponent() {
  for (size_t i = 0; i < 31; ++i) {
    gains[i].setRange(-12.0, 12.0, 0.5);
    gains[i].setValue(0.0);
    gains[i].setSliderStyle(juce::Slider::LinearVertical);
    gains[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(gains[i]);
    intensities[i].setRange(0.0, 100.0, 5.0);
    intensities[i].setValue(100.0);
    intensities[i].setSliderStyle(juce::Slider::LinearVertical);
    intensities[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(intensities[i]);
  }
  master.setRange(0.0, 100.0, 5.0);
  master.setValue(100.0);
  master.setSliderStyle(juce::Slider::LinearHorizontal);
  master.setTextBoxStyle(juce::Slider::TextBoxRight, false, 56, 20);
  addAndMakeVisible(master);

  auto mk = [this](juce::Label &l, const juce::String &t) {
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    l.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(l);
  };
  mk(gainTitle, "Ganancia (dB)");
  mk(intensityTitle, "Intensidad por banda (%)");
  mk(masterTitle, "Intensidad general:");
  mk(freqLabelsTitle, "20 Hz ... 20 kHz (1/3 octava)");
  mk(metersTitle, "Nivel por banda");
  addAndMakeVisible(meters);
}

int EqualizerComponent::colW() const {
  return (getWidth() - 16) / 31;
}

void EqualizerComponent::paint(juce::Graphics &g) {
  g.setColour(retro::face());
  g.fillAll();
  g.setColour(juce::Colours::black);
  g.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
  g.drawText("Ecualizador de 31 bandas", getLocalBounds().removeFromTop(22),
             juce::Justification::centred);
  // Etiquetas de frecuencia por columna.
  auto r = getLocalBounds().reduced(8);
  r.removeFromTop(46);
  g.setFont(juce::Font(juce::FontOptions(8.0f)));
  g.setColour(juce::Colours::black);
  for (size_t i = 0; i < 31; ++i) {
    float f = kEq31Freqs[i];
    juce::String t = f >= 1000.f
                         ? juce::String(juce::roundToInt(f / 1000.f)) + "k"
                         : juce::String(juce::roundToInt(f));
    g.drawText(t, 8 + int(i) * colW(), freqLabelsTop + 1, colW(), 12,
               juce::Justification::centred, false);
  }
}

void EqualizerComponent::resized() {
  auto r = getLocalBounds().reduced(8);
  r.removeFromTop(22);
  gainTitle.setBounds(r.removeFromTop(16).removeFromLeft(260));
  auto gainsRow = r.removeFromTop(int(getHeight() * 0.24));
  intensityTitle.setBounds(r.removeFromTop(14).removeFromLeft(320));
  auto intRow = r.removeFromTop(int(getHeight() * 0.24));
  auto freqs = r.removeFromTop(14);
  freqLabelsTop = freqs.getY();
  auto titleRow = r.removeFromTop(16);
  metersTitle.setBounds(titleRow.removeFromLeft(140));
  freqLabelsTitle.setBounds(titleRow.removeFromRight(260));
  auto metersRow = r.removeFromTop(int(getHeight() * 0.14));
  meters.setBounds(metersRow);
  r.removeFromTop(2);
  masterTitle.setBounds(r.removeFromLeft(160).reduced(0, 6));
  master.setBounds(r.reduced(0, 4));

  int w = gainsRow.getWidth() / 31;
  for (size_t i = 0; i < 31; ++i) {
    gains[i].setBounds(gainsRow.removeFromLeft(w).reduced(1, 0));
    intensities[i].setBounds(intRow.removeFromLeft(w).reduced(1, 0));
  }
}

Eq31State EqualizerComponent::getState() const {
  Eq31State s{};
  for (size_t i = 0; i < 31; ++i) {
    s.bands[i].gainDb = (float)gains[i].getValue();
    s.bands[i].intensity = (float)intensities[i].getValue() / 100.f;
  }
  s.masterIntensity = (float)master.getValue() / 100.f;
  return s;
}

void EqualizerComponent::setState(const Eq31State &s) {
  for (size_t i = 0; i < 31; ++i) {
    gains[i].setValue(s.bands[i].gainDb, juce::dontSendNotification);
    intensities[i].setValue(s.bands[i].intensity * 100.0,
                            juce::dontSendNotification);
  }
  master.setValue(s.masterIntensity * 100.0, juce::dontSendNotification);
}

} // namespace pg
