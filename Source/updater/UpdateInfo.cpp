#include "updater/UpdateInfo.h"

namespace pg::updater {
namespace {

int platformScore(const juce::String &name) {
  const auto n = name.toLowerCase();
#if JUCE_MAC
  if (n.endsWith(".dmg"))
    return 5;
  if (n.endsWith(".pkg"))
    return 4;
  if (n.endsWith(".zip") && n.contains("mac"))
    return 3;
  if (n.contains("mac") || n.contains("osx"))
    return 2;
#elif JUCE_WINDOWS
  if (n.endsWith(".exe"))
    return 5;
  if (n.endsWith(".msi"))
    return 4;
  if (n.contains("win"))
    return 2;
#else
  if (n.endsWith(".appimage"))
    return 5;
  if (n.endsWith(".deb"))
    return 4;
  if (n.endsWith(".rpm"))
    return 3;
  if (n.endsWith(".tar.gz") || n.endsWith(".tgz"))
    return 2;
  if (n.contains("linux"))
    return 1;
#endif
  return 0;
}

} // namespace

const UpdateAsset *UpdateInfo::assetForThisPlatform() const {
  const UpdateAsset *best = nullptr;
  int bestScore = 0;
  for (const auto &a : assets) {
    const int score = platformScore(a.name);
    if (score > bestScore) {
      bestScore = score;
      best = &a;
    }
  }
  return best;
}

} // namespace pg::updater
