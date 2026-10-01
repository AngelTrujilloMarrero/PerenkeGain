#pragma once
#include "updater/UpdateInfo.h"

namespace pg::updater {

// Lee la ultima release publica de GitHub y la compara con currentVersion.
// Bloquea: hay que llamarlo desde un hilo de fondo.
class GitHubReleaseClient {
public:
  static UpdateInfo fetchLatest(const juce::String &ownerRepo,
                                const juce::String &currentVersion);
};

} // namespace pg::updater
