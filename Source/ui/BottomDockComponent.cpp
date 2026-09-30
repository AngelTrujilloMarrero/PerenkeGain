#include "ui/BottomDockComponent.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

BottomDockComponent::BottomDockComponent() {
  addAndMakeVisible(help);
  addAndMakeVisible(eq);
  addAndMakeVisible(backBtn);
  eq.setVisible(false);
  backBtn.setVisible(false);
  help.onContinue = [this] {
    if (onHelpContinue)
      onHelpContinue();
  };
  backBtn.onClick = [this] { showHelp(); };
}

void BottomDockComponent::showHelp() {
  helpMode = true;
  help.setVisible(true);
  eq.setVisible(false);
  backBtn.setVisible(false);
  resized();
}

void BottomDockComponent::showEqualizer() {
  helpMode = false;
  help.setVisible(false);
  eq.setVisible(true);
  backBtn.setVisible(true);
  resized();
}

void BottomDockComponent::paint(juce::Graphics &g) {
  // Modo ecualizador: panel oscuro tipo mesa digital moderna.
  g.setColour(helpMode ? retro::face() : juce::Colour(0xFF1A1C22));
  g.fillAll();
}

void BottomDockComponent::resized() {
  auto r = getLocalBounds();
  if (helpMode) {
    help.setBounds(r);
    return;
  }
  backBtn.setBounds(r.removeFromTop(22).removeFromRight(84).reduced(2));
  eq.setBounds(r);
}

} // namespace pg
