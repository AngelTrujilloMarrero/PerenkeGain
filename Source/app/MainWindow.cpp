#include "app/MainWindow.h"
#include "app/EditorRootComponent.h"

namespace pg {

MainWindow::MainWindow()
    : DocumentWindow("PerenkeGain Editor",
                     juce::Colours::darkgrey.darker(0.85f),
                     DocumentWindow::allButtons) {
  setResizable(true, true);
  setContentOwned(new EditorRootComponent(), true);
  centreWithSize(1100, 720);
  setVisible(true);
}

void MainWindow::closeButtonPressed() {
  juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

} // namespace pg
