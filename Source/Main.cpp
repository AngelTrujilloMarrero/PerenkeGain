#include <JuceHeader.h>
#include "app/MainWindow.h"
#include "ui/RetroLookAndFeel.h"
#include "updater/AppVersion.h"

class PerenkeGainApp : public juce::JUCEApplication {
public:
  const juce::String getApplicationName() override { return "PerenkeGain"; }
  const juce::String getApplicationVersion() override {
    return pg::appVersion();
  }

  void initialise(const juce::String &) override {
    retro = std::make_unique<pg::RetroLookAndFeel>();
    juce::LookAndFeel::setDefaultLookAndFeel(retro.get());
    mainWindow = std::make_unique<pg::MainWindow>();
  }

  void shutdown() override {
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    mainWindow.reset();
    retro.reset();
  }

private:
  std::unique_ptr<pg::RetroLookAndFeel> retro;
  std::unique_ptr<pg::MainWindow> mainWindow;
};

START_JUCE_APPLICATION(PerenkeGainApp)
