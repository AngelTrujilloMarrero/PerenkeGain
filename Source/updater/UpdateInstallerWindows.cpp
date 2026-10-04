#include "updater/UpdateInstallerWindows.h"
#include <cstdlib>

#if JUCE_WINDOWS

namespace pg::updater::windows {
namespace {

bool runTool(const juce::String &program, const juce::StringArray &args) {
  juce::StringArray argv;
  argv.add(program);
  argv.addArray(args);
  juce::ChildProcess p;
  if (!p.start(argv))
    return false;
  return p.waitForProcessToFinish(180000) && p.getExitCode() == 0;
}

juce::File newTempDir() {
  auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                 .getChildFile("PerenkeGainUpdate_" +
                               juce::String::toHexString(
                                   juce::Random::getSystemRandom().nextInt64()));
  dir.createDirectory();
  return dir;
}

juce::String batQuote(const juce::String &s) { return "\"" + s + "\""; }

juce::File tarExe() {
  // tar.exe (bsdtar) viene con Windows 10 1803+. Se resuelve por ruta
  // absoluta para no depender del PATH; si no existe se deja que
  // CreateProcess lo busque en el PATH y si falla se aborta.
  auto sysTar = juce::File::getSpecialLocation(
                    juce::File::windowsSystemDirectory)
                    .getChildFile("tar.exe");
  if (sysTar.existsAsFile())
    return sysTar;
  return juce::File("tar.exe");
}

// Carpeta que contiene el exe nuevo (se busca por el nombre del exe actual
// para tolerar reubicaciones o renombrados).
juce::File locateNewRoot(const juce::File &dir,
                         const juce::String &exeName) {
  auto exes = dir.findChildFiles(juce::File::findFiles, true, exeName);
  if (exes.isEmpty())
    return {};
  return exes.getFirst().getParentDirectory();
}

juce::File writeHelperBat(const juce::File &newRoot, const juce::File &exe,
                          const juce::File &tempDir,
                          const juce::File &archive) {
  const int pid = (int)::GetCurrentProcessId();
  juce::String s;
  s << "@echo off\r\n";
  s << "setlocal\r\n";
  s << "set \"PID=" << pid << "\"\r\n";
  s << "set \"SRC=" << newRoot.getFullPathName() << "\"\r\n";
  s << "set \"DST=" << exe.getParentDirectory().getFullPathName() << "\"\r\n";
  s << "set \"EXE=" << exe.getFullPathName() << "\"\r\n";
  s << "set \"CLEAN=" << tempDir.getFullPathName() << "\"\r\n";
  // Espera a que la app termine (max ~2 min) y luego sustituye. El exe en
  // marcha no se puede sobrescribir, por eso hay que esperar primero.
  s << "set /a n=0\r\n";
  s << ":waitloop\r\n";
  s << "tasklist /FI \"PID eq %PID%\" 2>nul | findstr /I /C:\" %PID% \" "
       ">nul\r\n";
  s << "if errorlevel 1 goto donewait\r\n";
  s << "timeout /t 1 /nobreak >nul\r\n";
  s << "set /a n+=1\r\n";
  s << "if %n% geq 120 goto donewait\r\n";
  s << "goto waitloop\r\n";
  s << ":donewait\r\n";
  // Sin barra final en DST (\"...\\" romperia el entrecomillado).
  s << "xcopy \"%SRC%\\*\" \"%DST%\" /E /I /Y /R >nul\r\n";
  s << "start \"\" \"%EXE%\"\r\n";
  s << "rmdir /S /Q \"%CLEAN%\"\r\n";
  if (archive.existsAsFile())
    s << "rmdir /S /Q \""
      << archive.getParentDirectory().getFullPathName() << "\"\r\n";
  s << "del \"%~f0\"\r\n";

  auto script = tempDir.getChildFile(
      "perenkegain_update_" +
      juce::String::toHexString(
          juce::Random::getSystemRandom().nextInt64()) +
      ".bat");
  // CRLF explicito: es el formato nativo de los .bat (al reves que los .sh,
  // que exigen LF).
  if (!script.replaceWithText(s, false, false, "\r\n"))
    return {};
  return script;
}

} // namespace

bool isSupported(const juce::File &targetExe) {
  // Hace falta poder escribir la carpeta (se copian los ficheros nuevos
  // sobre ella). Sin esto el ayudante fallaria y relanzaria la version vieja.
  return targetExe.existsAsFile() &&
         targetExe.getParentDirectory().hasWriteAccess();
}

bool installAndRelaunch(const juce::File &archive) {
  auto target = juce::File::getSpecialLocation(
      juce::File::currentExecutableFile);
  if (!archive.existsAsFile() || !isSupported(target))
    return false;

  auto temp = newTempDir();
  if (!temp.isDirectory())
    return false;

  if (!runTool(tarExe().getFullPathName(),
               {"-xf", archive.getFullPathName(), "-C",
                temp.getFullPathName()})) {
    temp.deleteRecursively();
    return false;
  }

  auto newRoot = locateNewRoot(temp, target.getFileName());
  if (!newRoot.isDirectory()) {
    temp.deleteRecursively();
    return false;
  }

  auto script = writeHelperBat(newRoot, target, temp, archive);
  if (script == juce::File{} || !script.existsAsFile()) {
    temp.deleteRecursively();
    return false;
  }

  // Lanza el ayudante desacoplado del proceso actual (sobrevive al cierre):
  // "start" lo independiza y /min evita ventanazos.
  juce::String cmd = "cmd /c start \"\" /min " + batQuote(script.getFullPathName());
  // Si el lanzamiento falla hay que devolver false: si no, el llamante
  // cerraria la app y nada la reinstalaria ni relanzaria.
  const int rc = std::system(cmd.toRawUTF8());
  if (rc != 0) {
    script.deleteFile();
    temp.deleteRecursively();
    return false;
  }
  return true;
}

} // namespace pg::updater::windows

#endif // JUCE_WINDOWS
