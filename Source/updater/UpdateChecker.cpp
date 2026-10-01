#include "updater/UpdateChecker.h"
#include "updater/AppVersion.h"
#include "updater/GitHubReleaseClient.h"
#include "updater/UpdateConfig.h"
#include <memory>
#include <thread>

namespace pg::updater {

void UpdateChecker::checkAsync(Callback onResult) const {
  auto callback = std::make_shared<Callback>(std::move(onResult));
  std::thread([callback] {
    const auto info =
        GitHubReleaseClient::fetchLatest(juce::String(kOwnerRepo),
                                         pg::appVersion());
    juce::MessageManager::callAsync([callback, info] {
      if (*callback)
        (*callback)(info);
    });
  }).detach();
}

} // namespace pg::updater
