#pragma once
#include <JuceHeader.h>

namespace pg {

// Abre ventanas de diálogo modales con el LookAndFeel moderno global.
namespace dialogs {
void show(const juce::String &title, juce::Component *content, int w, int h);
} // namespace dialogs

} // namespace pg
