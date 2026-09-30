#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "audio/WaveformComponent.h"
#include "audio/TrackMarkersComponent.h"
#include "audio/TransportComponent.h"
#include "ui/AdvancedPanel.h"

namespace pg {

// Contenedor raíz del editor: layout + cableado abrir/play/stop.
class EditorRootComponent : public juce::Component {
public:
  EditorRootComponent();
  void resized() override;

private:
  void openFile();

  AudioEngine engine;
  WaveformComponent wave;
  TrackMarkersComponent markers;
  TransportComponent transport;
  AdvancedPanel advanced;
  juce::TextButton openButton{"Abrir audio..."};
  std::unique_ptr<juce::FileChooser> chooser;
};

} // namespace pg
