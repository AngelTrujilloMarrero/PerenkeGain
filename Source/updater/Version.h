#pragma once
#include <JuceHeader.h>

namespace pg::updater {

struct Version {
  int major = 0, minor = 0, patch = 0;
};

// Acepta "v1.2.3", "1.2", "1.2.3-beta.1", "1.2.3+build".
Version parseVersion(const juce::String &tag);

// -1 si a < b, 0 si son iguales, 1 si a > b.
int compareVersions(const Version &a, const Version &b);

// true si candidate es estrictamente mas nueva que current.
bool isNewerVersion(const juce::String &candidate, const juce::String &current);

// "1.2.3" (sin prefijo v ni sufijos).
juce::String normaliseVersion(const juce::String &tag);

} // namespace pg::updater
