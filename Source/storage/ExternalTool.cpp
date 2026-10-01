#include "storage/ExternalTool.h"

namespace pg {

juce::File ExternalTool::find(const juce::String &name) {
  juce::StringArray dirs = juce::StringArray::fromTokens(
      juce::SystemStats::getEnvironmentVariable("PATH", {}), ":", "");
  // Al abrir la app desde el Finder el PATH no trae estas rutas.
  dirs.add("/opt/homebrew/bin");
  dirs.add("/usr/local/bin");
  dirs.add("/opt/local/bin");
  dirs.add("/usr/bin");
  dirs.add("/bin");

  for (auto &dir : dirs) {
    if (dir.isEmpty())
      continue;
    juce::File f = juce::File(dir).getChildFile(name);
    if (f.existsAsFile())
      return f;
  }
  return {};
}

} // namespace pg
