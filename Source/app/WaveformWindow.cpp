#include "app/WaveformWindow.h"
#include "ui/ModernLookAndFeel.h"

namespace pg {

WaveformWindow::WaveformWindow(AudioEngine &e, MarkerModel &m)
    : juce::DocumentWindow("Editor de onda - PerenkeGain", modern::surface(),
                           juce::DocumentWindow::allButtons) {
  content = new WaveformEditorComponent(e, m);
  setUsingNativeTitleBar(true);
  setResizable(true, true);
  setContentOwned(content, true);
  centreWithSize(880, 460);
  setVisible(false); // se abre desde el boton "Editor de onda..."
}

void WaveformWindow::closeButtonPressed() { setVisible(false); }

} // namespace pg
