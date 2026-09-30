#include "app/MainWindow.h"
#include "audio/WaveformComponent.h"
#include "audio/TransportComponent.h"
#include "ui/AdvancedPanel.h"

namespace pg {

MainWindow::MainWindow()
    : DocumentWindow("PerenkeGain Editor",
                     juce::Colours::darkgrey.darker(0.85f),
                     DocumentWindow::allButtons) {
  setResizable(true, true);
  centreWithSize(1100, 700);

  auto *root = new juce::Component();
  root->addAndMakeVisible(*new WaveformComponent());
  root->addAndMakeVisible(*new TransportComponent());
  root->addAndMakeVisible(*new AdvancedPanel());
  setContentOwned(root, true);
  setVisible(true);
}

void MainWindow::closeButtonPressed() {
  juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

} // namespace pg
