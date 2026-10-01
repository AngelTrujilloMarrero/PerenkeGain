#include "app/MainWindow.h"
#include "app/EditorRootComponent.h"

namespace pg {

MainWindow::MainWindow()
    : DocumentWindow("PerenkeGain - Editor de Sonido",
                     juce::Colour(0xFFD4D0C8),
                     DocumentWindow::allButtons) {
  setUsingNativeTitleBar(true);
  setResizable(true, true);
  setContentOwned(new EditorRootComponent(), true);
  centreWithSize(900, 880);
  setVisible(true);
}

void MainWindow::closeButtonPressed() {
  juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

} // namespace pg
