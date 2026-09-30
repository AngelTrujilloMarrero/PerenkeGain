#include "ui/HelpInfoPanel.h"
#include "types/Text.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

HelpInfoPanel::HelpInfoPanel() {
  text.setText(
      PG_T("La secci\u00f3n Avanzada permite aplicar filtros a la "
           "grabaci\u00f3n. Puede escuchar el resultado inmediatamente, "
           "guardar el archivo con los filtros aplicados, en disco duro, o "
           "guardar los cambios en un nuevo archivo y a\u00f1adir todos los "
           "filtros deseados. Todos los filtros se aplican sobre la marcha y "
           "de forma independiente, por lo que puede comparar en cualquier "
           "momento la grabaci\u00f3n original y la filtrada (A/B)."),
      juce::dontSendNotification);
  text.setInterceptsMouseClicks(false, false);
  text.setFont(juce::Font(juce::FontOptions(12.0f)));
  addAndMakeVisible(text);
  addAndMakeVisible(tip);
  addAndMakeVisible(cont);
  tip.setToggleState(false, juce::dontSendNotification);
  cont.onClick = [this] {
    if (onContinue)
      onContinue();
  };
}

void HelpInfoPanel::resized() {
  auto r = getLocalBounds().reduced(4);
  auto bottom = r.removeFromBottom(26);
  tip.setBounds(
      bottom.removeFromLeft(juce::jmin(280, bottom.getWidth())).reduced(2, 0));
  cont.setBounds(bottom.removeFromRight(110).reduced(2, 0));
  text.setBounds(r);
}

} // namespace pg
