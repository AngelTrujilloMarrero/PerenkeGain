#include "app/DialogLauncher.h"

namespace pg::dialogs {

void show(const juce::String &title, juce::Component *content, int w, int h) {
  juce::DialogWindow::LaunchOptions o;
  o.dialogTitle = title;
  o.dialogBackgroundColour = juce::Colour(0xFFD4D0C8);
  o.content.setOwned(content);
  o.content->setSize(w, h);
  o.escapeKeyTriggersCloseButton = true;
  o.useNativeTitleBar = true;
  o.launchAsync();
}

} // namespace pg::dialogs
