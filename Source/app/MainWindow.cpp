#include "app/MainWindow.h"
#include "app/EditorRootComponent.h"

namespace pg {

MainWindow::MainWindow()
    : DocumentWindow("PerenkeGain - Editor y Reproductor de Sonido",
                     juce::Colour(0xFF0B0C10),
                     DocumentWindow::allButtons) {
  setUsingNativeTitleBar(true);
  setResizable(true, true);
#if JUCE_ANDROID
  setResizeLimits(320, 240, 16000, 16000);
  setContentOwned(new EditorRootComponent(), true);
  // En Android, JUCE no es fullscreen por defecto: sin esto la ventana se
  // crea como overlay flotante con el tamano de centroWithSize (940x980).
  setFullScreen(true);
#else
  setResizeLimits(900, 620, 16000, 16000);
  setContentOwned(new EditorRootComponent(), true);
  centreWithSize(940, 980);
#endif
  setVisible(true);
}

void MainWindow::closeButtonPressed() {
  juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

} // namespace pg
