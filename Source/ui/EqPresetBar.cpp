#include "ui/EqPresetBar.h"
#include "dsp/EqPresets.h"
#include "types/Text.h"

namespace pg {

namespace {
constexpr int kCustomId = 1000; // id del item "Personalizado"
}

EqPresetBar::EqPresetBar() {
  title.setText(PG_T("Preset:"), juce::dontSendNotification);
  title.setFont(juce::Font(juce::FontOptions(11.0f, juce::Font::bold)));
  title.setColour(juce::Label::textColourId, juce::Colour(0xFFF2F3F6));
  title.setInterceptsMouseClicks(false, false);
  addAndMakeVisible(title);

  for (size_t i = 0; i < kEqPresets.size(); ++i)
    box.addItem(PG_T(kEqPresets[i].name), (int)i + 1);
  box.addSeparator();
  box.addItem(PG_T("Personalizado"), kCustomId);
  box.setSelectedId(1, juce::dontSendNotification);

  box.onChange = [this] {
    const int id = box.getSelectedId();
    if (id == kCustomId) {
      // Elegir "Personalizado" a mano no hace nada: vuelve al preset.
      box.setSelectedId(lastPreset + 1, juce::dontSendNotification);
      return;
    }
    if (id >= 1) {
      lastPreset = id - 1;
      if (onPresetChosen)
        onPresetChosen(lastPreset);
    }
  };
  addAndMakeVisible(box);
}

void EqPresetBar::resized() {
  auto r = getLocalBounds();
  title.setBounds(r.removeFromLeft(52));
  box.setBounds(r.removeFromLeft(220));
}

void EqPresetBar::select(int index) {
  lastPreset = juce::jlimit(0, (int)kEqPresets.size() - 1, index);
  box.setSelectedId(lastPreset + 1, juce::dontSendNotification);
}

void EqPresetBar::showCustom() {
  box.setSelectedId(kCustomId, juce::dontSendNotification);
}

bool EqPresetBar::isCustom() const {
  return box.getSelectedId() == kCustomId;
}

} // namespace pg
