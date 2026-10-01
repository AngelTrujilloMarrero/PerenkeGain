#include "ui/WaveformEditorComponent.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

WaveformEditorComponent::WaveformEditorComponent(AudioEngine &e, MarkerModel &m)
    : engine(e), model(m) {
  addAndMakeVisible(wave);
  addAndMakeVisible(meter);
  meter.attach(&engine.bandAnalyzer());
  for (auto *c : {&cutBox, &fadeIn, &fadeOut})
    addAndMakeVisible(*c);
  addAndMakeVisible(splitB);
  addAndMakeVisible(saveB);
  addAndMakeVisible(escalaTitle);
  escalaTitle.setText("Escala:", juce::dontSendNotification);
  escalaTitle.setFont(juce::Font(juce::FontOptions(12.0f)));
  addAndMakeVisible(escala);
  escala.setRange(1.0, 50.0, 1.0);
  escala.setValue(10.0);
  escala.setSliderStyle(juce::Slider::LinearHorizontal);
  escala.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 46, 20);
  escala.onValueChange = [this] {
    wave.setVerticalZoom((float)escala.getValue() / 10.0f);
  };

  cutBox.onClick = [this] {
    const int d = engine.activeDeck();
    double a = engine.getPositionSec(d), b = a;
    if (wave.hasSelection()) {
      auto s = wave.getSelection();
      a = s.first;
      b = s.second;
      wave.clearSelection();
    }
    model.addMarker(a, b);
  };
  splitB.onClick = [this] {
    if (onRequestSplit)
      onRequestSplit();
  };
  saveB.onClick = [this] {
    if (onRequestSave)
      onRequestSave();
  };
  wave.onSeek = [this](double sec) {
    const int d = engine.activeDeck();
    if (engine.hasFile(d))
      engine.setCurrentPosition(d, sec);
  };

  model.addChangeListener(this);
  changeListenerCallback(&model);
  startTimerHz(30);
}

WaveformEditorComponent::~WaveformEditorComponent() {
  model.removeChangeListener(this);
}

void WaveformEditorComponent::changeListenerCallback(
    juce::ChangeBroadcaster *) {
  if (model.file() != lastFile) {
    lastFile = model.file();
    if (lastFile.existsAsFile())
      wave.openFile(lastFile);
  }
  wave.setMarkers(model.markers());
  repaint();
}

void WaveformEditorComponent::timerCallback() {
  if (!isShowing())
    return;
  const int d = engine.activeDeck();
  wave.setPlayhead(engine.hasFile(d) ? engine.getPositionSec(d) : -1.0);
}

void WaveformEditorComponent::paint(juce::Graphics &g) {
  g.setColour(retro::face());
  g.fillAll();
}

void WaveformEditorComponent::resized() {
  auto r = getLocalBounds().reduced(6);
  auto left = r.removeFromLeft(200);
  left.removeFromTop(4);
  cutBox.setBounds(left.removeFromTop(26));
  fadeIn.setBounds(left.removeFromTop(26));
  fadeOut.setBounds(left.removeFromTop(26));
  left.removeFromTop(8);
  escalaTitle.setBounds(left.removeFromTop(24).removeFromLeft(60));
  escala.setBounds(left.removeFromTop(26).removeFromTop(24));
  left.removeFromTop(10);
  splitB.setBounds(left.removeFromTop(28).reduced(6, 0));
  saveB.setBounds(left.removeFromTop(28).reduced(6, 2));

  meter.setBounds(r.removeFromRight(40).reduced(2, 6));
  wave.setBounds(r.reduced(4, 6));
}

} // namespace pg
