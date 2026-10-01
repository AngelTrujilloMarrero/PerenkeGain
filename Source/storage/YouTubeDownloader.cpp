#include "storage/YouTubeDownloader.h"
#include "storage/ExternalTool.h"
#include "types/Text.h"

namespace pg {

namespace {
bool isAudio(const juce::File &f) {
  return f.hasFileExtension("mp3") || f.hasFileExtension("wav") ||
         f.hasFileExtension("m4a") || f.hasFileExtension("aac") ||
         f.hasFileExtension("opus") || f.hasFileExtension("webm") ||
         f.hasFileExtension("flac") || f.hasFileExtension("ogg");
}

// Saca una ruta de audio valida de una linea de yt-dlp (Destination, ruta
// suelta, "already been downloaded").
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
} // namespace

bool YouTubeDownloader::available(juce::File &ytdlp, juce::File &ffmpeg) {
  ytdlp = ExternalTool::find("yt-dlp");
  ffmpeg = ExternalTool::find("ffmpeg");
  return ytdlp != juce::File{} && ffmpeg != juce::File{};
}

YouTubeResult YouTubeDownloader::download(
    const juce::String &url, const juce::File &outDir,
    const std::function<void(float)> &onProgress) {
  juce::ignoreUnused(onProgress);
  YouTubeResult r;
  auto ytdlp = ExternalTool::find("yt-dlp");
  if (ytdlp == juce::File{}) {
    r.error = "yt-dlp no encontrado";
    return r;
  }
  auto ffmpeg = ExternalTool::find("ffmpeg");
  if (ffmpeg == juce::File{}) {
    r.error = "ffmpeg no encontrado";
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
  juce::StringArray args{ytdlp.getFullPathName(),
                         "-x",
                         "--audio-format",
                         "mp3",
                         "--audio-quality",
                         "0",
                         "--ffmpeg-location",
                         ffmpeg.getParentDirectory().getFullPathName(),
                         "--no-playlist",
                         "--no-warnings",
                         "--print",
                         "after_move:filepath",
                         "--no-simulate",
                         "-o",
                         tmpl,
                         url};

  juce::ChildProcess proc;
  if (!proc.start(args)) {
    r.error = PG_T("No se pudo lanzar yt-dlp");
    return r;
  }
  const juce::String output = proc.readAllProcessOutput();
  const int code = proc.getExitCode();
  if (code != 0) {
    r.error = "yt-dlp termino con codigo " + juce::String(code);
    return r;
  }

  // 1) Ruta en la salida de yt-dlp.
  juce::File found;
  for (const auto &line : juce::StringArray::fromLines(output)) {
    const juce::File f = pathFromLine(line);
    if (f != juce::File{})
      found = f;
  }

  // 2) Fichero nuevo mas reciente de la carpeta (prefiere mp3).
  if (found == juce::File{}) {
    juce::Array<juce::File> fresh;
    for (const auto &f : outDir.findChildFiles(juce::File::findFiles, false))
      if (!before.contains(f.getFileName()) && isAudio(f))
        fresh.add(f);
    for (const auto &f : fresh)
      if (f.hasFileExtension("mp3") &&
          (found == juce::File{} ||
           f.getLastModificationTime() > found.getLastModificationTime()))
        found = f;
    if (found == juce::File{})
      for (const auto &f : fresh)
        if (found == juce::File{} ||
            f.getLastModificationTime() > found.getLastModificationTime())
          found = f;
  }

  if (found == juce::File{}) {
    r.error = PG_T("No se encontro el archivo descargado");
    return r;
  }
  r.ok = true;
  r.file = found;
  return r;
}

} // namespace pg
