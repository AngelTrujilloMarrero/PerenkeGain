#include <JuceHeader.h>
#include "app/MainWindow.h"

class PerenkeGainApp : public juce::JUCEApplication {
public:
  const juce::String getApplicationName() override { return "PerenkeGain"; }
  const juce::String getApplicationVersion() override { return "0.1.0"; }

  void initialise(const juce::String &) override {
    mainWindow = std::make_unique<pg::MainWindow>();
  }
  void shutdown() override { mainWindow.reset(); }

private:
  std::unique_ptr<pg::MainWindow> mainWindow;
};

START_JUCE_APPLICATION(PerenkeGainApp)
