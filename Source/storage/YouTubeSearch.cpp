#include "storage/YouTubeSearch.h"
#include "storage/ExternalTool.h"
#include "storage/YouTubeAndroid.h"
#include "types/Text.h"

namespace pg {

std::vector<YouTubeSearchItem>
YouTubeSearch::search(const juce::String &query, int maxResults,
                      juce::String &error) {
#if JUCE_ANDROID
  return YouTubeAndroid::search(query, maxResults, error);
#else
  std::vector<YouTubeSearchItem> out;
  auto ytdlp = ExternalTool::find("yt-dlp");
  if (ytdlp == juce::File{}) {
    error = "yt-dlp no encontrado";
    return out;
  }
  // Busqueda en YouTube Music (devuelve tambien albumes/artistas que filtramos).
  const juce::String searchUrl =
      "https://music.youtube.com/search?q=" +
      juce::URL::addEscapeChars(query, true);
  juce::StringArray args{ytdlp.getFullPathName(),
                         "--flat-playlist",
                         "--skip-download",
                         "--no-warnings",
                         "--print",
                         "%(title)s\x1F%(url)s",
                         searchUrl};
  juce::ChildProcess proc;
  if (!proc.start(args)) {
    error = PG_T("No se pudo lanzar yt-dlp");
    return out;
  }
  const juce::String output = proc.readAllProcessOutput();
  const int limit = juce::jmax(1, maxResults);
  for (const auto &line : juce::StringArray::fromLines(output)) {
    if ((int)out.size() >= limit)
      break;
    auto parts = juce::StringArray::fromTokens(line, "\x1F", "");
    parts.trim();
    if (parts.size() < 2)
      continue;
    const juce::String title = parts[0];
    const juce::String url = parts[1];
    if (title.isEmpty() || title == "NA")
      continue;
    if (!url.contains("watch?v=")) // solo pistas, no albumes/artistas
      continue;
    out.push_back({title, url, {}});
  }
  if (out.empty() && error.isEmpty()) {
    for (const auto &l : juce::StringArray::fromLines(output)) {
      const auto t = l.trim();
      if (t.startsWithIgnoreCase("ERROR")) {
        error = t;
        break;
      }
    }
    if (error.isEmpty()) {
      const int code = (int)proc.getExitCode();
      error = code != 0 ? PG_T("yt-dlp terminó con código ") +
                              juce::String(code)
                        : juce::String("Sin resultados");
    }
  }
  return out;
#endif
}

} // namespace pg
