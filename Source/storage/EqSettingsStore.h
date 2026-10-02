#pragma once
#include <JuceHeader.h>
#include "types/EqTypes.h"

namespace pg {

// Persistencia de la última ecualización (gains, mutes, master y preset).
// Va a un PropertiesFile de usuario ("PerenkeGain.eq"), así sobrevive
// a reinicios sin tocar el proyecto ni la sesión.
struct EqSavedState {
  Eq31State state{};
  int preset = 0;    // índice en kEqPresets
  bool custom = false; // true = el usuario retocó faders a mano
  bool hasData = false;
};

class EqSettingsStore {
public:
  EqSavedState load();
  void save(const Eq31State &s, int preset, bool custom);

private:
  std::unique_ptr<juce::PropertiesFile> open();
};

} // namespace pg
