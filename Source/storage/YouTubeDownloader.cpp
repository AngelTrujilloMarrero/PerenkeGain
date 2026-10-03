#include "storage/YouTubeDownloader.h"
#include "storage/ExternalTool.h"
#include "storage/YouTubeAndroid.h"
#include "types/Text.h"

namespace pg {

namespace {
bool isAudio(const juce::File &f) {
  return f.hasFileExtension("mp3") || f.hasFileExtension("wav") ||
         f.hasFileExtension("m4a") || f.hasFileExtension("aac") ||
         f.hasFileExtension("opus") || f.hasFileExtension("webm") ||
         f.hasFileExtension("flac") || f.hasFileExtension("ogg");
}

// Saca una ruta de audio valida de una linea de yt-dlp.
juce::File pathFromLine(const juce::String &raw) {
  juce::String s = raw.trim();
  const int di = s.indexOf("Destination:");
  if (di >= 0)
    s = s.substring(di + 12).trim();
  else if (s.contains("has already been downloaded"))
    s = s.upToFirstOccurrenceOf(" has already been downloaded", false, false)
            .trim();
  if (s.startsWithChar('[')) {
    const int e = s.indexOfChar(']');
    if (e >= 0)
      s = s.substring(e + 1).trim();
  }
  juce::File f(s);
  return (f.existsAsFile() && isAudio(f)) ? f : juce::File{};
}

// Resumen del fallo de yt-dlp: ultimas lineas utiles (sin progreso).
juce::String errorSummary(const juce::String &output, int code) {
  juce::StringArray keep;
  for (const auto &l : juce::StringArray::fromLines(output)) {
    const auto t = l.trim();
    if (t.isEmpty())
      continue;
    if (t.startsWith("[download]") && t.containsChar('%'))
      continue;
    keep.add(t);
  }
  juce::String msg = "yt-dlp terminó con código " + juce::String(code);
  if (!keep.isEmpty()) {
    juce::StringArray tail;
    for (int i = juce::jmax(0, keep.size() - 4); i < keep.size(); ++i)
      tail.add(keep[i]);
    msg = tail.joinIntoString("\n");
  }
  if (msg.containsIgnoreCase("unable to extract") ||
      msg.containsIgnoreCase("player response") ||
      msg.containsIgnoreCase("sign in") ||
      msg.containsIgnoreCase("failed to extract"))
    msg += "\n" + ExternalTool::ytDlpUpdateHint();
  return msg;
}

// Convierte cualquier audio a MP3 con ffmpeg (no depende de ffprobe).
bool convertToMp3(const juce::File &in, const juce::File &out) {
  const juce::File ffmpeg = ExternalTool::find("ffmpeg");
  if (ffmpeg == juce::File{})
    return false;
  juce::ChildProcess p;
  juce::StringArray a{ffmpeg.getFullPathName(), "-y", "-loglevel", "error",
                      "-i", in.getFullPathName(), "-vn", "-c:a", "libmp3lame",
                      "-q:a", "0", out.getFullPathName()};
  if (!p.start(a))
    return false;
  p.waitForProcessToFinish(180000);
  return p.getExitCode() == 0 && out.existsAsFile() && out.getSize() > 0;
}
} // namespace

bool YouTubeDownloader::available(juce::File &ytdlp, juce::File &ffmpeg) {
#if JUCE_ANDROID
  ytdlp = juce::File{};
  ffmpeg = juce::File{};
  return true;
#else
  ytdlp = ExternalTool::find("yt-dlp");
  ffmpeg = ExternalTool::find("ffmpeg");
  return ytdlp != juce::File{} && ffmpeg != juce::File{};
#endif
}

YouTubeResult YouTubeDownloader::download(
    const juce::String &url, const juce::File &outDir,
    const std::function<void(float)> &onProgress) {
  juce::ignoreUnused(onProgress);
#if JUCE_ANDROID
  return YouTubeAndroid::download(url, outDir);
#else
  YouTubeResult r;
  auto ytdlp = ExternalTool::find("yt-dlp");
  if (ytdlp == juce::File{}) {
    r.error = "yt-dlp no encontrado";
    return r;
  }
  if (ExternalTool::find("ffmpeg") == juce::File{}) {
    r.error = "ffmpeg no encontrado";
    return r;
  }
  // Un yt-dlp viejo falla siempre ("Please sign in"): se avisa sin esperar.
  juce::String ytVer;
  if (ExternalTool::isYtDlpOutdated(ytdlp, &ytVer)) {
    r.error = PG_T("yt-dlp desactualizado (") + ytVer + PG_T("). ") +
              ExternalTool::ytDlpUpdateHint();
    return r;
  }
  if (!outDir.isDirectory() && !outDir.createDirectory()) {
    r.error = "No se pudo crear la carpeta destino";
    return r;
  }

  juce::StringArray before;
  for (const auto &f : outDir.findChildFiles(juce::File::findFiles, false))
    before.add(f.getFileName());

  const juce::String tmpl =
      outDir.getChildFile("%(title)s.%(ext)s").getFullPathName();
  // Descarga el mejor audio SIN postprocesar (evita ffprobe); la conversion a
  // MP3 la hacemos nosotros con ffmpeg.
  juce::StringArray args{ytdlp.getFullPathName(), "-f",    "bestaudio",
                         "--no-playlist",       "--no-warnings", "-o",
                         tmpl,                  url};

  juce::ChildProcess proc;
  if (!proc.start(args)) {
    r.error = PG_T("No se pudo lanzar yt-dlp");
    return r;
  }
  const juce::String output = proc.readAllProcessOutput();
  const int code = proc.getExitCode();
  if (code != 0) {
    r.error = errorSummary(output, code);
    return r;
  }

  // 1) Ruta en la salida de yt-dlp.
  juce::File found;
  for (const auto &line : juce::StringArray::fromLines(output)) {
    const juce::File f = pathFromLine(line);
    if (f != juce::File{})
      found = f;
  }
  // 2) Fichero nuevo mas reciente de la carpeta.
  if (found == juce::File{}) {
    for (const auto &f : outDir.findChildFiles(juce::File::findFiles, false))
      if (!before.contains(f.getFileName()) && isAudio(f) &&
          (found == juce::File{} ||
           f.getLastModificationTime() > found.getLastModificationTime()))
        found = f;
  }
  if (found == juce::File{}) {
    r.error = PG_T("No se encontro el archivo descargado");
    return r;
  }

  // Convierte a MP3.
  if (!found.hasFileExtension("mp3")) {
    const juce::File mp3 = found.getSiblingFile(
        found.getFileNameWithoutExtension() + ".mp3");
    if (!convertToMp3(found, mp3)) {
      r.error = PG_T("No se pudo convertir a MP3 (revisa ffmpeg/libmp3lame)");
      return r;
    }
    found.deleteFile();
    found = mp3;
  }

  r.ok = true;
  r.file = found;
  return r;
#endif
}

} // namespace pg
