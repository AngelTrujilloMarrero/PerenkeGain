#include "updater/GitHubReleaseClient.h"
#include "updater/AppVersion.h"
#include "updater/UpdateConfig.h"
#include "updater/Version.h"

namespace pg::updater {

namespace {

void readAssets(const juce::var &json, UpdateInfo &info) {
  if (auto *arr = json.getProperty("assets", {}).getArray()) {
    for (const auto &item : *arr) {
      UpdateAsset a;
      a.name = item.getProperty("name", "").toString();
      a.downloadUrl = item.getProperty("browser_download_url", "").toString();
      a.sizeBytes = (juce::int64)item.getProperty("size", 0);
      if (a.downloadUrl.isNotEmpty())
        info.assets.push_back(a);
    }
  }
}

} // namespace

UpdateInfo GitHubReleaseClient::fetchLatest(const juce::String &ownerRepo,
                                            const juce::String &currentVersion) {
  UpdateInfo info;
  info.currentVersion = normaliseVersion(currentVersion);

  juce::URL url(juce::String(kApiBase) + "/repos/" + ownerRepo +
                "/releases/latest");
  int status = 0;
  auto options =
      juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
          .withExtraHeaders("User-Agent: PerenkeGain-Updater/" +
                            pg::appVersion() +
                            "\r\nAccept: application/vnd.github+json")
          .withConnectionTimeoutMs(kTimeoutMs)
          .withStatusCode(&status);

  auto stream = url.createInputStream(options);
  if (stream == nullptr)
    return info;

  auto json = juce::JSON::parse(stream->readEntireStreamAsString());
  if (!json.isObject())
    return info;

  info.checked = true;
  info.latestVersion =
      normaliseVersion(json.getProperty("tag_name", "").toString());
  info.releaseName = json.getProperty("name", "").toString();
  info.releaseNotes = json.getProperty("body", "").toString();
  info.releaseUrl = json.getProperty("html_url", "").toString();
  readAssets(json, info);

  info.available =
      info.latestVersion.isNotEmpty() &&
      isNewerVersion(info.latestVersion, info.currentVersion);
  return info;
}

} // namespace pg::updater
