#pragma once
#include <JuceHeader.h>

namespace pg::updater {

// Instalacion automatica: extrae el paquete descargado, sustituye la app
// actual y la vuelve a lanzar. En macOS reemplaza el bundle .app; en Linux
// sustituye el binario (o el AppImage). Funciona con un script ayudante que
// espera a que este proceso termine.
class UpdateInstaller {
public:
  // Fichero/carpeta que hay que reemplazar (bundle .app o binario).
  static juce::File currentInstallTarget();

  // false si la app corre desde una ruta que no se puede sustituir
  // (p. ej. un .app translocado por Gatekeeper).
  static bool isSelfInstallSupported();

  // Devuelve true si ya dejo programada la sustitucion + reinicio;
  // el llamante debe cerrar la applicacion a continuacion.
  static bool installAndRelaunch(const juce::File &archive);
};

} // namespace pg::updater
