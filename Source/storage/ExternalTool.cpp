#include "storage/ExternalTool.h"
#include "types/Text.h"

namespace pg {

juce::File ExternalTool::managedToolsDir() {
  return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
      .getChildFile("PerenkeGain")
      .getChildFile("tools");
}

juce::Array<juce::File> ExternalTool::localToolsDirs() {
  juce::Array<juce::File> dirs;
  // 1) Descargas de ToolInstaller: gana al bundle para que las
  //    actualizaciones de la app se noten.
  dirs.add(managedToolsDir());
  // 2) Bundle: tools/ al lado del ejecutable (Windows) o dentro de
  //    Contents/MacOS/tools (macOS).
  const juce::File exeDir =
      juce::File::getSpecialLocation(juce::File::currentExecutableFile)
          .getParentDirectory();
  dirs.add(exeDir.getChildFile("tools"));
  return dirs;
}

juce::File ExternalTool::find(const juce::String &name) {
  juce::StringArray candidates;
  candidates.add(name);
#if JUCE_WINDOWS
  candidates.add(name + ".exe"); // yt-dlp.exe, ffmpeg.exe...
#endif

  // 1) Carpetas propias: lo que empaqueta la release o instala la app.
  for (const auto &dir : localToolsDirs()) {
    for (const auto &n : candidates) {
      juce::File f = dir.getChildFile(n);
      if (f.existsAsFile()) {
#if !JUCE_WINDOWS
        f.setExecutePermission(true); // por si el zip perdio el bit x
#endif
        return f;
      }
    }
  }

#if JUCE_WINDOWS
  const juce::String sep = ";";
#else
  const juce::String sep = ":";
#endif
  juce::StringArray dirs = juce::StringArray::fromTokens(
      juce::SystemStats::getEnvironmentVariable("PATH", {}), sep, "");
#if JUCE_WINDOWS
  // Instalaciones habituales en Windows: winget, chocolatey y scoop.
  dirs.add(juce::SystemStats::getEnvironmentVariable("LOCALAPPDATA", {}) +
           "\\Microsoft\\WindowsApps");
  dirs.add(juce::SystemStats::getEnvironmentVariable("ProgramData", {}) +
           "\\chocolatey\\bin");
  dirs.add(juce::File::getSpecialLocation(juce::File::userHomeDirectory)
               .getChildFile("scoop\\shims")
               .getFullPathName());
#else
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
#endif

  for (auto &dir : dirs) {
    if (dir.isEmpty())
      continue;
    for (const auto &n : candidates) {
      juce::File f = juce::File(dir).getChildFile(n);
      if (f.existsAsFile())
        return f;
    }
  }
  return {};
}

juce::String ExternalTool::toolVersion(const juce::File &tool) {
  juce::ChildProcess p;
  juce::StringArray args{tool.getFullPathName(), "--version"};
  if (!p.start(args))
    return {};
  const juce::String out = p.readAllProcessOutput().trim();
  if (out.isEmpty())
    return {};
  return juce::StringArray::fromLines(out)[0].trim();
}

bool ExternalTool::isYtDlpOutdated(const juce::File &ytdlp,
                                   juce::String *foundVersion) {
  juce::String v = toolVersion(ytdlp).trim();
  if (foundVersion != nullptr)
    *foundVersion = v;
  // yt-dlp versiona por fecha: "2026.08.19". Se ignora texto extra.
  if (v.containsChar(' '))
    v = v.upToFirstOccurrenceOf(" ", false, true).trim();
  if (!v.containsOnly("0123456789."))
    return false;
  juce::StringArray parts =
      juce::StringArray::fromTokens(v, ".", "");
  parts.removeEmptyStrings();
  if (parts.size() < 3)
    return false;
  const int y = parts[0].getIntValue();
  const int m = parts[1].getIntValue();
  const int d = parts[2].getIntValue();
  if (y < 2020 || m < 1 || m > 12 || d < 1 || d > 31)
    return false;
  const juce::Time released(y, m - 1, d, 0, 0, 0);
  if (released.toMilliseconds() == 0)
    return false;
  const juce::Time limit =
      juce::Time::getCurrentTime() - juce::RelativeTime::days(180);
  return released < limit;
}

juce::String ExternalTool::ytDlpUpdateHint() {
#if JUCE_LINUX
  return PG_T("yt-dlp desactualizado: ejecuta 'yt-dlp -U' o instala el "
              "binario oficial en ~/.local/bin (github.com/yt-dlp/yt-dlp)");
#elif JUCE_MAC
  return PG_T("yt-dlp desactualizado: ejecuta 'brew upgrade yt-dlp'");
#elif JUCE_WINDOWS
  return PG_T("yt-dlp desactualizado: ejecuta 'winget upgrade yt-dlp'");
#else
  return PG_T("yt-dlp desactualizado: actualiza yt-dlp a la última versión");
#endif
}

} // namespace pg
