#include "ui/LevelerPanel.h"
#include "types/Text.h"

namespace pg {

namespace {
void initSlider(juce::Slider &s, double lo, double hi, double step,
                const juce::String &suffix, double value) {
  s.setRange(lo, hi, step);
  s.setValue(value, juce::dontSendNotification);
  s.setSliderStyle(juce::Slider::LinearHorizontal);
  s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 78, 20);
  s.setTextValueSuffix(suffix);
}
} // namespace

LevelerPanel::LevelerPanel() {
  addAndMakeVisible(enable);
  enable.setColour(juce::ToggleButton::textColourId, juce::Colour(0xFFF2F3F6));
  enable.onClick = [this] { pushParams(); };

  speed.addItem(PG_T("Suave"), 1);
  speed.addItem(PG_T("Media"), 2);
  speed.addItem(PG_T("R\u00e1pida"), 3);
  speed.setSelectedId(2, juce::dontSendNotification);
  speed.setColour(juce::ComboBox::textColourId, juce::Colour(0xFFF2F3F6));
  speed.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF14151A));
  speed.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF9AA0AC));
  speed.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xFFF2F3F6));
  speed.onChange = [this] { pushParams(); };
  addAndMakeVisible(speed);

  for (auto *l : {&targetL, &speedL, &boostL, &cutL, &ceilingL, &gateL}) {
    l->setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
    l->setColour(juce::Label::textColourId, juce::Colour(0xFFF2F3F6));
    addAndMakeVisible(*l);
  }
  targetL.setText(PG_T("Objetivo:"), juce::dontSendNotification);
  speedL.setText(PG_T("Velocidad:"), juce::dontSendNotification);
  boostL.setText(PG_T("Subida:"), juce::dontSendNotification);
  cutL.setText(PG_T("Bajada:"), juce::dontSendNotification);
  ceilingL.setText(PG_T("Techo:"), juce::dontSendNotification);
  gateL.setText(PG_T("Puerta:"), juce::dontSendNotification);

  initSlider(target, -30.0, -9.0, 0.5, " LUFS", -14.0);
  initSlider(boost, 0.0, 24.0, 0.5, " dB", 12.0);
  initSlider(cut, 0.0, 36.0, 0.5, " dB", 24.0);
  initSlider(ceiling, -6.0, 0.0, 0.1, " dBFS", -1.0);
  initSlider(gate, -80.0, -40.0, 1.0, " LUFS", -60.0);
  for (auto *s : {&target, &boost, &cut, &ceiling, &gate}) {
    s->setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xFFF2F3F6));
    s->setColour(juce::Slider::textBoxBackgroundColourId,
                 juce::Colour(0xFF14151A));
    s->setColour(juce::Slider::textBoxOutlineColourId,
                 juce::Colour(0xFF9AA0AC));
    s->onValueChange = [this] { pushParams(); };
    addAndMakeVisible(*s);
  }

  for (auto *l : {&momentaryL, &shortTermL, &gainL, &peakL}) {
    l->setFont(juce::Font(juce::FontOptions(11.0f)));
    l->setColour(juce::Label::backgroundColourId, juce::Colour(0xFF14151A));
    l->setColour(juce::Label::textColourId, juce::Colour(0xFF7CFF3C));
    l->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(*l);
  }

  startTimerHz(15);
}

void LevelerPanel::setEngine(AudioEngine *e) {
  engine = e;
  if (engine != nullptr) {
    const LevelerParams p = engine->getLevelerParams();
    enable.setToggleState(p.enabled, juce::dontSendNotification);
    target.setValue(p.targetLufs, juce::dontSendNotification);
    boost.setValue(p.maxBoostDb, juce::dontSendNotification);
    cut.setValue(p.maxCutDb, juce::dontSendNotification);
    ceiling.setValue(p.ceilingDb, juce::dontSendNotification);
    gate.setValue(p.gateLufs, juce::dontSendNotification);
  }
}

void LevelerPanel::pushParams() {
  if (engine == nullptr)
    return;
  LevelerParams p;
  p.enabled = enable.getToggleState();
  p.targetLufs = (float)target.getValue();
  p.maxBoostDb = (float)boost.getValue();
  p.maxCutDb = (float)cut.getValue();
  p.ceilingDb = (float)ceiling.getValue();
  p.gateLufs = (float)gate.getValue();
  p.attackSec = speedAttack();
  p.releaseSec = speedRelease();
  engine->setLevelerParams(p);
}

float LevelerPanel::speedAttack() const {
  switch (speed.getSelectedId()) {
  case 1: return 0.35f;
  case 3: return 0.08f;
  default: return 0.2f;
  }
}

float LevelerPanel::speedRelease() const {
  switch (speed.getSelectedId()) {
  case 1: return 2.5f;
  case 3: return 0.6f;
  default: return 1.5f;
  }
}

void LevelerPanel::timerCallback() {
  if (!isShowing() || engine == nullptr)
    return;
  const LevelerMeters m = engine->getLevelerMeters();
  auto fmt = [](float v) { return juce::String(v, 1); };
  momentaryL.setText(PG_T("Mom.") + juce::String(" ") + fmt(m.momentaryLufs) +
                         " LUFS",
                     juce::dontSendNotification);
  shortTermL.setText(PG_T("Corto") + juce::String(" ") + fmt(m.shortTermLufs) +
                         " LUFS",
                     juce::dontSendNotification);
  gainL.setText(PG_T("Ganancia") + juce::String(" ") + fmt(m.appliedGainDb) +
                    " dB",
                juce::dontSendNotification);
  peakL.setText(PG_T("Pico") + juce::String(" ") + fmt(m.peakDb) + " dBFS",
                juce::dontSendNotification);
}

void LevelerPanel::paint(juce::Graphics &g) {
  g.setColour(juce::Colour(0xFF1A1C22));
  g.fillAll();
}

void LevelerPanel::resized() {
  auto r = getLocalBounds().reduced(6, 2);
  auto row1 = r.removeFromTop(24);
  enable.setBounds(row1.removeFromLeft(110));
  targetL.setBounds(row1.removeFromLeft(64));
  target.setBounds(row1.removeFromLeft(190));
  row1.removeFromLeft(10);
  speedL.setBounds(row1.removeFromLeft(70));
  speed.setBounds(row1.removeFromLeft(120));

  r.removeFromTop(2);
  auto row2 = r.removeFromTop(24);
  auto place = [&row2](juce::Label &l, juce::Slider &s) {
    l.setBounds(row2.removeFromLeft(52));
    s.setBounds(row2.removeFromLeft(150).reduced(0, 1));
  };
  place(boostL, boost);
  place(cutL, cut);
  place(ceilingL, ceiling);
  place(gateL, gate);

  r.removeFromTop(2);
  auto row3 = r.removeFromTop(20);
  const int w = juce::jmax(1, row3.getWidth() / 4);
  momentaryL.setBounds(row3.removeFromLeft(w).reduced(2, 0));
  shortTermL.setBounds(row3.removeFromLeft(w).reduced(2, 0));
  gainL.setBounds(row3.removeFromLeft(w).reduced(2, 0));
  peakL.setBounds(row3.reduced(2, 0));
}

} // namespace pg
