#pragma once
#include <JuceHeader.h>

namespace pg::updater::windows {

// ¿Puede esta instalacion sustituirse a si misma en Windows? (el exe
// existe y su carpeta tiene permiso de escritura).
bool isSupported(const juce::File &targetExe);

// Extrae el zip, programa la sustitucion + reinicio con un .bat ayudante
// que espera a que este proceso termine. Devuelve true si quedo programado
// (el llamante debe cerrar la app a continuacion).
bool installAndRelaunch(const juce::File &archive);

} // namespace pg::updater::windows
