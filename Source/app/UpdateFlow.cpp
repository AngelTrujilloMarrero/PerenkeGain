#include "app/EditorRootComponent.h"
#include "app/DialogLauncher.h"
#include "types/Text.h"
#include "ui/UpdateDialog.h"

namespace pg {

void EditorRootComponent::checkForUpdates() {
#if JUCE_ANDROID
  return;
#else
  auto safe = juce::Component::SafePointer<EditorRootComponent>(this);
  updateChecker.checkAsync([safe](updater::UpdateInfo info) {
    if (safe != nullptr && info.available)
      safe->showUpdateAvailable(info);
  });
#endif
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

} // namespace pg
