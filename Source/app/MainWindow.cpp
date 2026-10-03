#include "app/MainWindow.h"
#include "app/EditorRootComponent.h"

namespace pg {

MainWindow::MainWindow()
    : DocumentWindow("PerenkeGain - Editor y Reproductor de Sonido",
                     juce::Colour(0xFF0B0C10),
                     DocumentWindow::allButtons) {
  setUsingNativeTitleBar(true);
  setResizable(true, true);
  setResizeLimits(900, 620, 16000, 16000);
  setContentOwned(new EditorRootComponent(), true);
  centreWithSize(940, 980);
  setVisible(true);
}

void MainWindow::closeButtonPressed() {
  juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

} // namespace pg
