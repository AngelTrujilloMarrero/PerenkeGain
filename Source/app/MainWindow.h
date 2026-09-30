#pragma once
#include <JuceHeader.h>

namespace pg {

class MainWindow : public juce::DocumentWindow {
public:
  MainWindow();
  void closeButtonPressed() override;
};

} // namespace pg
