#pragma once
#include <JuceHeader.h>

namespace pg {

// Persistencia ligera proyectos (SQLite en F4, MVP en memoria).
class ProjectStore {
public:
  void setLastFormat(int id) { lastFormat = id; }
  int getLastFormat() const { return lastFormat; }

private:
  int lastFormat = 1;
};

} // namespace pg
