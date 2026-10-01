#include "updater/Version.h"

namespace pg::updater {

Version parseVersion(const juce::String &tag) {
  juce::String s = tag.trim();
  if (s.startsWithChar('v') || s.startsWithChar('V'))
    s = s.substring(1);
  s = s.upToFirstOccurrenceOf("-", false, false);
  s = s.upToFirstOccurrenceOf("+", false, false);

  juce::StringArray parts;
  parts.addTokens(s, ".", "");
  parts.removeEmptyStrings();

  Version v;
  if (parts.size() > 0)
    v.major = parts[0].getIntValue();
  if (parts.size() > 1)
    v.minor = parts[1].getIntValue();
  if (parts.size() > 2)
    v.patch = parts[2].getIntValue();
  return v;
}

int compareVersions(const Version &a, const Version &b) {
  if (a.major != b.major)
    return a.major < b.major ? -1 : 1;
  if (a.minor != b.minor)
    return a.minor < b.minor ? -1 : 1;
  if (a.patch != b.patch)
    return a.patch < b.patch ? -1 : 1;
  return 0;
}

bool isNewerVersion(const juce::String &candidate, const juce::String &current) {
  return compareVersions(parseVersion(candidate), parseVersion(current)) > 0;
}

juce::String normaliseVersion(const juce::String &tag) {
  auto v = parseVersion(tag);
  return juce::String(v.major) + "." + juce::String(v.minor) + "." +
         juce::String(v.patch);
}

} // namespace pg::updater
