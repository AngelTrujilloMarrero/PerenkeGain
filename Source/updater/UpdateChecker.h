#pragma once
#include "updater/UpdateInfo.h"
#include <functional>

namespace pg::updater {

// Lanza la comprobacion en un hilo de fondo y devuelve el resultado en el
// hilo de mensajes (seguro para tocar UI).
class UpdateChecker {
public:
  using Callback = std::function<void(UpdateInfo)>;

  void checkAsync(Callback onResult) const;
};

} // namespace pg::updater
