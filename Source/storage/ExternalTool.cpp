#include "storage/ExternalTool.h"

namespace pg {

juce::File ExternalTool::find(const juce::String &name) {
  juce::StringArray dirs = juce::StringArray::fromTokens(
      juce::SystemStats::getEnvironmentVariable("PATH", {}), ":", "");
  // Al abrir desde el escritorio el PATH viene minimo: se anaden las rutas
  // habituales de Homebrew, pip (usuario), snap y Linuxbrew.
  dirs.add(juce::File::getSpecialLocation(juce::File::userHomeDirectory)
               .getChildFile(".local/bin")
               .getFullPathName());
  dirs.add("/opt/homebrew/bin");
  dirs.add("/usr/local/bin");
  dirs.add("/usr/local/sbin");
  dirs.add("/opt/local/bin");
  dirs.add("/home/linuxbrew/.linuxbrew/bin");
  dirs.add("/snap/bin");
  dirs.add("/var/lib/snapd/snap/bin");
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
