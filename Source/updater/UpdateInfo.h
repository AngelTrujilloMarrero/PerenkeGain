#pragma once
#include <JuceHeader.h>
#include <vector>

namespace pg::updater {

struct UpdateAsset {
  juce::String name;
  juce::String downloadUrl;
  juce::int64 sizeBytes = 0;
};

struct UpdateInfo {
  bool checked = false;   // se pudo consultar GitHub
  bool available = false; // hay una version mas nueva
  juce::String currentVersion;
  juce::String latestVersion;
  juce::String releaseName;
  juce::String releaseNotes;
  juce::String releaseUrl;
  std::vector<UpdateAsset> assets;

  // Mejor binario para esta plataforma (o nullptr si la release no trae uno).
  const UpdateAsset *assetForThisPlatform() const;
};

} // namespace pg::updater
