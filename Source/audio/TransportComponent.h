#pragma once
#include <JuceHeader.h>
#include "types/Text.h"

namespace pg {

// Transporte del editor original: botones cuadrados pequeños ▶ ■ ⏭.
class TransportComponent : public juce::Component {
public:
  TransportComponent();
  void resized() override;

  juce::TextButton play{PG_T("▶")};
  juce::TextButton stop{PG_T("■")};
  juce::TextButton toEnd{PG_T("▶|")};

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportComponent)
};

} // namespace pg
