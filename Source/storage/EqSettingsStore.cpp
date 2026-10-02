#include "storage/EqSettingsStore.h"

namespace pg {

namespace {
constexpr const char *kGains = "gainsDb";
constexpr const char *kMutes = "mutes";
constexpr const char *kMaster = "master";
constexpr const char *kPreset = "preset";
constexpr const char *kCustom = "custom";
} // namespace

std::unique_ptr<juce::PropertiesFile> EqSettingsStore::open() {
  juce::PropertiesFile::Options opts;
  opts.applicationName = "PerenkeGain";
  opts.filenameSuffix = ".settings";
  opts.folderName = "PerenkeGain";
  opts.osxLibrarySubFolder = "Application Support";
  opts.storageFormat = juce::PropertiesFile::storeAsXML;
  return std::make_unique<juce::PropertiesFile>(opts);
}

EqSavedState EqSettingsStore::load() {
  EqSavedState out;
  auto props = open();
  if (props == nullptr || !props->containsKey(kGains))
    return out; // sin datos: el EQ arranca con el preset por defecto

  const auto gains = juce::StringArray::fromTokens(
      props->getValue(kGains), ",", {});
  const juce::String mutes = props->getValue(kMutes);
  if (gains.size() < 31)
    return out;

  for (int i = 0; i < 31; ++i) {
    out.state.bands[(size_t)i].gainDb =
        juce::jlimit(-12.0f, 12.0f, gains[i].getFloatValue());
    out.state.bands[(size_t)i].intensity = 1.0f;
    out.state.bands[(size_t)i].enabled =
        i >= mutes.length() || mutes[i] != '1';
  }
  out.state.masterIntensity =
      juce::jlimit(0.0f, 1.0f, (float)props->getDoubleValue(kMaster, 1.0));
  out.preset = props->getIntValue(kPreset, 0);
  out.custom = props->getBoolValue(kCustom, true);
  out.hasData = true;
  return out;
}

void EqSettingsStore::save(const Eq31State &s, int preset, bool custom) {
  auto props = open();
  if (props == nullptr)
    return;
  juce::StringArray gains;
  juce::String mutes;
  for (int i = 0; i < 31; ++i) {
    gains.add(juce::String(s.bands[(size_t)i].gainDb, 1));
    mutes += s.bands[(size_t)i].enabled ? "0" : "1";
  }
  props->setValue(kGains, gains.joinIntoString(","));
  props->setValue(kMutes, mutes);
  props->setValue(kMaster, (double)s.masterIntensity);
  props->setValue(kPreset, preset);
  props->setValue(kCustom, custom);
  props->saveIfNeeded();
}

} // namespace pg
