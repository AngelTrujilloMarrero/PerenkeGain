#include "app/EditorRootComponent.h"
#include "app/DialogLauncher.h"
#include "types/Text.h"
#include "ui/RetroLookAndFeel.h"
#include "ui/UpdateDialog.h"

namespace pg {

EditorRootComponent::EditorRootComponent()
    : controller(engine, wave, info),
      ticker(engine, info, cyan, wave) {
  addAndMakeVisible(filePath);
  filePath.setColour(juce::Label::backgroundColourId, juce::Colours::white);
  filePath.setColour(juce::Label::textColourId, juce::Colours::black);
  filePath.setFont(juce::Font(juce::FontOptions(12.0f)));
  filePath.setText("Sin archivo", juce::dontSendNotification);

  updateBanner.onShowDetails = [this] { openUpdateDialog(); };
  updateBanner.onDismiss = [this] {
    updateBanner.setVisible(false);
    layoutRows();
  };
  addChildComponent(updateBanner);

  addAndMakeVisible(transport);
  addAndMakeVisible(info);
  addAndMakeVisible(cyan);
  addAndMakeVisible(wave);
  addAndMakeVisible(meter);
  addAndMakeVisible(dock);
  for (auto *b : {&openB, &saveB, &splitB, &advB, &closeB, &skipB})
    addAndMakeVisible(*b);
  for (auto *c : {&cutBox, &fadeIn, &fadeOut})
    addAndMakeVisible(*c);
  addAndMakeVisible(escalaTitle);
  escalaTitle.setText("Escala:", juce::dontSendNotification);
  escalaTitle.setFont(juce::Font(juce::FontOptions(12.0f)));
  addAndMakeVisible(escala);
  escala.setRange(1.0, 50.0, 1.0);
  escala.setValue(10.0);
  escala.setSliderStyle(juce::Slider::LinearHorizontal);
  escala.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 46, 20);
  escala.onValueChange = [this] {
    wave.setVerticalZoom((float)escala.getValue() / 10.0f);
  };

  controller.onFileLoaded = [this](const juce::String &path) {
    filePath.setText(path, juce::dontSendNotification);
  };
  dock.eq.mixer.attach(&engine.bandAnalyzer());
  meter.attach(&engine.bandAnalyzer());
  dock.leveler.setEngine(&engine);
  // El EQ del panel afecta al audio en reproducción.
  dock.eq.onStateChanged = [this] { engine.setEqState(dock.eq.getState()); };
  engine.setEqState(dock.eq.getState());
  wireButtons();

  // Arranque con archivo por línea de comandos: PerenkeGain archivo.wav
  if (auto *app = juce::JUCEApplication::getInstance()) {
    const auto &params = app->getCommandLineParameterArray();
    if (!params.isEmpty()) {
      auto f = juce::File(params[0]);
      if (f.existsAsFile())
        controller.openFilePath(f);
    }
  }

  // Al arrancar: comprobar en segundo plano si hay una version nueva.
  checkForUpdates();
}

void EditorRootComponent::checkForUpdates() {
  auto safe = juce::Component::SafePointer<EditorRootComponent>(this);
  updateChecker.checkAsync([safe](updater::UpdateInfo info) {
    if (safe != nullptr && info.available)
      safe->showUpdateAvailable(info);
  });
}

void EditorRootComponent::showUpdateAvailable(
    const updater::UpdateInfo &info) {
  pendingUpdate = info;
  updateBanner.setVersion(info.latestVersion, info.currentVersion);
  updateBanner.setVisible(true);
  layoutRows();
}

void EditorRootComponent::openUpdateDialog() {
  auto *dlg = new UpdateDialog(pendingUpdate);
  dialogs::show(PG_T("Actualizaci\u00f3n de PerenkeGain"), dlg, 500, 440);
}

void EditorRootComponent::wireButtons() {
  openB.onClick = [this] { controller.openAudio(); };
  saveB.onClick = [this] { controller.openSave(); };
  splitB.onClick = [this] { controller.openSplitter(); };
  advB.onClick = [this] { controller.openBatchNormalize(); };
  closeB.onClick = [this] { controller.quitEditor(); };
  skipB.onClick = [this] {
    engine.setCurrentPosition(
        juce::jmin(engine.getPositionSec() + 5.0, engine.getLengthSec()));
  };
  transport.play.onClick = [this] {
    if (engine.hasFile()) {
      if (engine.isPlaying())
        engine.stop();
      else
        engine.play();
    }
  };
  transport.stop.onClick = [this] { engine.stop(); };
  transport.toEnd.onClick = [this] {
    engine.setCurrentPosition(engine.getLengthSec());
  };
  cutBox.onClick = [this] { controller.addMarkerAtPlayhead(); };
  // Clic simple sobre la onda: salta a esa posición.
  wave.onSeek = [this](double sec) {
    if (engine.hasFile())
      engine.setCurrentPosition(sec);
  };
  // Clic/arrastre sobre la barra cian: salta a esa fracción del tema.
  cyan.onSeekFraction = [this](double f) {
    if (engine.hasFile())
      engine.setCurrentPosition(f * engine.getLengthSec());
  };
}

void EditorRootComponent::layoutRows() {
  auto r = getLocalBounds().reduced(3);
  if (updateBanner.isVisible())
    updateBanner.setBounds(r.removeFromTop(36).reduced(0, 2));
  auto row1 = r.removeFromTop(32);
  transport.setBounds(row1.removeFromRight(110).reduced(2, 0));
  filePath.setBounds(row1.reduced(0, 2));
  info.setBounds(r.removeFromTop(38));
  auto row3 = r.removeFromTop(26).reduced(0, 2);
  skipB.setBounds(row3.removeFromRight(46));
  cyan.setBounds(row3.reduced(0, 3));

  dock.setBounds(r.removeFromBottom(dock.preferredHeight()));

  auto right = r.removeFromRight(160);
  auto rightButtons = {&openB, &saveB, &splitB, &advB, &closeB};
  const int slot = juce::jmax(1, right.getHeight() / (int)rightButtons.size());
  for (auto *b : rightButtons)
    b->setBounds(right.removeFromTop(slot).reduced(4, 2));

  auto left = r.removeFromLeft(190).reduced(0, 6);
  left.removeFromTop(4);
  cutBox.setBounds(left.removeFromTop(26));
  fadeIn.setBounds(left.removeFromTop(26));
  fadeOut.setBounds(left.removeFromTop(26));
  left.removeFromTop(8);
  escalaTitle.setBounds(left.removeFromTop(24).removeFromLeft(60));
  escala.setBounds(left.removeFromTop(26).removeFromTop(24));
  meter.setBounds(r.removeFromRight(40).reduced(2, 6));
  wave.setBounds(r.reduced(4, 6));
}

void EditorRootComponent::resized() { layoutRows(); }

} // namespace pg
