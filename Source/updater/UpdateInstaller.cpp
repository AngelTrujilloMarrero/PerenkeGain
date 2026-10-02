#include "updater/UpdateInstaller.h"
#include <cstdlib>
#if JUCE_MAC || JUCE_LINUX
#include <unistd.h>
#endif

namespace pg::updater {

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

juce::String shellQuote(const juce::String &s) {
  return "'" + s.replace("'", "'\\''") + "'";
}

juce::File newTempDir() {
  auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                 .getChildFile("PerenkeGainUpdate_" +
                               juce::String::toHexString(
                                   juce::Random::getSystemRandom().nextInt64()));
  dir.createDirectory();
  return dir;
}

#if JUCE_MAC
juce::File appBundleFor(const juce::File &exe) {
  return exe.getParentDirectory().getParentDirectory().getParentDirectory();
}
#endif

juce::File locateExtractedArtifact(const juce::File &dir) {
#if JUCE_MAC
  auto apps = dir.findChildFiles(juce::File::findDirectories, true, "*.app");
  if (!apps.isEmpty())
    return apps.getFirst();
#elif JUCE_LINUX
  auto bins = dir.findChildFiles(juce::File::findFiles, true, "PerenkeGain");
  if (!bins.isEmpty())
    return bins.getFirst();
  auto any = dir.findChildFiles(juce::File::findFiles, true, "*");
  if (!any.isEmpty())
    return any.getFirst();
#endif
  return {};
}

juce::File writeHelperScript(const juce::File &newArtifact,
                             const juce::File &target,
                             const juce::File &tempDir,
                             const juce::File &archive) {
  juce::String s;
  s << "#!/bin/sh\n";
  // Espera a que la app termine, con limite para no quedarse colgada
  // para siempre si el dialogo se cerro sin salir (reuso de PID, etc).
  // getpid() solo existe en POSIX: en Windows este script no se usa.
#if JUCE_MAC || JUCE_LINUX
  const int pid = (int)::getpid();
#else
  const int pid = 0;
#endif
  s << "n=0\n";
  s << "while kill -0 " << pid
    << " 2>/dev/null; do\n"
       "  sleep 0.3\n"
       "  n=$((n + 1))\n"
       "  if [ \"$n\" -ge 200 ]; then break; fi\n"
       "done\n";
  s << "TARGET=" << shellQuote(target.getFullPathName()) << "\n";
  s << "NEW=" << shellQuote(newArtifact.getFullPathName()) << "\n";
#if JUCE_MAC
  // Copia primero a un nombre temporal; solo sustituye si la copia fue bien.
  s << "if cp -R \"$NEW\" \"$TARGET.new\" 2>/dev/null; then\n";
  s << "  rm -rf \"$TARGET\"\n";
  s << "  mv \"$TARGET.new\" \"$TARGET\" 2>/dev/null || "
       "rm -rf \"$TARGET.new\"\n";
  s << "  xattr -dr com.apple.quarantine \"$TARGET\" 2>/dev/null\n";
  s << "fi\n";
  s << "open \"$TARGET\"\n";
#elif JUCE_LINUX
  s << "if cp -f \"$NEW\" \"$TARGET.new\" 2>/dev/null; then\n";
  s << "  chmod +x \"$TARGET.new\"\n";
  s << "  mv -f \"$TARGET.new\" \"$TARGET\"\n";
  s << "fi\n";
  s << "nohup \"$TARGET\" >/dev/null 2>&1 &\n";
#endif
  s << "rm -rf " << shellQuote(tempDir.getFullPathName()) << "\n";
  if (archive.existsAsFile())
    s << "rm -rf " << shellQuote(archive.getParentDirectory().getFullPathName())
      << "\n";
  s << "rm -f \"$0\"\n";

  // Nombre unico por intento: evita colisiones entre reintentos o
  // instancias y ataques por enlace simbolico en /tmp con nombre fijo.
  auto script =
      juce::File::getSpecialLocation(juce::File::tempDirectory)
          .getChildFile("perenkegain_update_" +
                        juce::String::toHexString(
                            juce::Random::getSystemRandom().nextInt64()) +
                        ".sh");
  // LF explicito: replaceWithText usa CRLF por defecto y /bin/sh falla
  // con "Syntax error" ante retornos de carro (la actualizacion nunca
  // se aplicaba y la app se cerraba sin mas).
  if (!script.replaceWithText(s, false, false, "\n"))
    return {};
  script.setExecutePermission(true);
  return script;
}

} // namespace

juce::File UpdateInstaller::currentInstallTarget() {
  auto exe = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
#if JUCE_MAC
  auto bundle = appBundleFor(exe);
  if (bundle.getFileName().endsWithIgnoreCase(".app"))
    return bundle;
  return exe;
#elif JUCE_LINUX
  auto appImage = juce::SystemStats::getEnvironmentVariable("APPIMAGE", {});
  if (appImage.isNotEmpty()) {
    juce::File f(appImage);
    if (f.existsAsFile())
      return f;
  }
  return exe;
#else
  return exe;
#endif
}

bool UpdateInstaller::isSelfInstallSupported() {
  auto target = currentInstallTarget();
#if JUCE_MAC
  return target.getFileName().endsWithIgnoreCase(".app") &&
         !target.getFullPathName().contains("AppTranslocation");
#elif JUCE_LINUX
  // Hace falta poder escribir el directorio (se crea "$TARGET.new" y se
  // renombra sobre el binario). Sin esto el ayudante fallaba en silencio
  // y relanzaba la version vieja.
  return target.existsAsFile() &&
         target.getParentDirectory().hasWriteAccess();
#else
  return false;
#endif
}

bool UpdateInstaller::installAndRelaunch(const juce::File &archive) {
  if (!archive.existsAsFile() || !isSelfInstallSupported())
    return false;

  auto target = currentInstallTarget();
  auto temp = newTempDir();
  if (!temp.isDirectory())
    return false;

  bool extracted = false;
#if JUCE_MAC
  extracted = runTool("/usr/bin/ditto",
                      {"-x", "-k", archive.getFullPathName(),
                       temp.getFullPathName()});
#elif JUCE_LINUX
  extracted = runTool("/bin/tar",
                      {"-xzf", archive.getFullPathName(), "-C",
                       temp.getFullPathName()});
#endif
  auto artifact = extracted ? locateExtractedArtifact(temp) : juce::File{};
  if (!artifact.exists()) {
    temp.deleteRecursively();
    return false;
  }

  auto script = writeHelperScript(artifact, target, temp, archive);
  if (script == juce::File{} || !script.existsAsFile()) {
    temp.deleteRecursively();
    return false;
  }

  // Lanza el ayudante desacoplado del proceso actual (sobrevive al cierre).
  juce::String cmd = "nohup /bin/sh " + shellQuote(script.getFullPathName()) +
                     " >/dev/null 2>&1 &";
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

} // namespace pg::updater
