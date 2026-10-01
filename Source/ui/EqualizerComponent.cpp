#include "ui/EqualizerComponent.h"
#include "dsp/EqPresets.h"
#include "types/Text.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

EqualizerComponent::EqualizerComponent() {
  master.setRange(0.0, 100.0, 5.0);
  master.setValue(100.0);
  master.setSliderStyle(juce::Slider::LinearHorizontal);
  master.setTextBoxStyle(juce::Slider::TextBoxRight, false, 56, 20);
  master.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
  master.setColour(juce::Slider::textBoxBackgroundColourId,
                   juce::Colour(0xFF14151A));
  addAndMakeVisible(master);

  auto mk = [this](juce::Label &l, const juce::String &t) {
    l.setText(t, juce::dontSendNotification);
    l.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    l.setColour(juce::Label::textColourId, juce::Colour(0xFFF2F3F6));
    l.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(l);
  };
  mk(masterTitle, "Intensidad general:");
  mk(freqLabelsTitle, "20 Hz ... 20 kHz (1/3 octava)");
  readout.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
  readout.setText("--", juce::dontSendNotification);
  readout.setColour(juce::Label::backgroundColourId, juce::Colour(0xFF14151A));
  readout.setColour(juce::Label::textColourId, juce::Colour(0xFF7CFF3C));
  readout.setColour(juce::Label::outlineColourId, juce::Colours::black);
  readout.setInterceptsMouseClicks(false, false);
  addAndMakeVisible(readout);

  master.onValueChange = [this] {
    if (onStateChanged)
      onStateChanged();
  };

  addAndMakeVisible(mixer);
  mixer.onReadout = [this](const juce::String &s) {
    readout.setText(s, juce::dontSendNotification);
    presetBar.showCustom(); // el usuario retoco un fader a mano
    if (onStateChanged)
      onStateChanged();
  };

  addAndMakeVisible(presetBar);
  presetBar.onPresetChosen = [this](int i) { applyPreset(i); };
}

int EqualizerComponent::colW() const {
  return (getWidth() - 16) / 31;
}

void EqualizerComponent::paint(juce::Graphics &g) {
  g.setColour(juce::Colour(0xFF1E2028)); // panel oscuro tipo mesa digital
  g.fillAll();
  g.setColour(juce::Colour(0xFFF2F3F6));
  g.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
  g.drawText("Ecualizador de 31 bandas", getLocalBounds().removeFromTop(22),
             juce::Justification::centred);
  // Etiquetas de frecuencia bajo las tiras.
  g.setFont(juce::Font(juce::FontOptions(8.0f)));
  g.setColour(juce::Colour(0xFF9AA0AC));
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
  auto titleRow = r.removeFromTop(22);
  readout.setBounds(titleRow.removeFromLeft(230).reduced(4, 3));
  freqLabelsTitle.setBounds(titleRow.removeFromRight(260).reduced(0, 5));
  presetBar.setBounds(r.removeFromTop(26));
  r.removeFromTop(2);
  auto masterRow = r.removeFromBottom(28);
  masterTitle.setBounds(masterRow.removeFromLeft(160).reduced(0, 6));
  master.setBounds(masterRow.reduced(0, 4));
  auto freqs = r.removeFromBottom(14);
  freqLabelsTop = freqs.getY();
  r.removeFromBottom(4);
  mixer.setBounds(r);
}

Eq31State EqualizerComponent::getState() const {
  Eq31State s{};
  for (size_t i = 0; i < 31; ++i) {
    s.bands[i].gainDb = mixer.gainDb((int)i);
    s.bands[i].intensity = 1.0f; // fuera de la UI: siempre al 100 %
    s.bands[i].enabled = !mixer.isMuted((int)i);
  }
  s.masterIntensity = (float)master.getValue() / 100.f;
  return s;
}

void EqualizerComponent::setState(const Eq31State &s) {
  for (size_t i = 0; i < 31; ++i) {
    mixer.setGainDb((int)i, s.bands[i].gainDb);
    mixer.setMuted((int)i, !s.bands[i].enabled);
  }
  master.setValue(s.masterIntensity * 100.0, juce::dontSendNotification);
}

void EqualizerComponent::applyPreset(int index) {
  index = juce::jlimit(0, (int)kEqPresets.size() - 1, index);
  Eq31State s = eqPresetState(kEqPresets[(size_t)index]);
  s.masterIntensity = (float)master.getValue() / 100.f; // conserva el master
  setState(s);
  presetBar.select(index);
  readout.setText(PG_T("Preset: ") + PG_T(kEqPresets[(size_t)index].name),
                  juce::dontSendNotification);
  // setState es programatico (no dispara onEdit): avisa a mano al DSP.
  if (onStateChanged)
    onStateChanged();
}

} // namespace pg
