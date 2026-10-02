#include <JuceHeader.h>
#include "app/MainWindow.h"
#include "ui/ModernLookAndFeel.h"
#include "updater/AppVersion.h"

class PerenkeGainApp : public juce::JUCEApplication {
public:
  const juce::String getApplicationName() override { return "PerenkeGain"; }
  const juce::String getApplicationVersion() override {
    return pg::appVersion();
  }

  void initialise(const juce::String &) override {
    modern = std::make_unique<pg::ModernLookAndFeel>();
    juce::LookAndFeel::setDefaultLookAndFeel(modern.get());
    mainWindow = std::make_unique<pg::MainWindow>();
  }

  void shutdown() override {
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    mainWindow.reset();
    modern.reset();
  }

private:
  std::unique_ptr<pg::ModernLookAndFeel> modern;
  std::unique_ptr<pg::MainWindow> mainWindow;
};

START_JUCE_APPLICATION(PerenkeGainApp)
