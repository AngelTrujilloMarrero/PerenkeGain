#include "storage/ToolInstaller.h"
#include "storage/ExternalTool.h"
#include "types/Text.h"
#include <thread>

namespace pg {

namespace {

constexpr bool kArm =
#if defined(__aarch64__) || defined(__arm64__)
    true;
#else
    false;
#endif

constexpr const char *kYtDlp =
#if JUCE_WINDOWS
    "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe";
#elif JUCE_MAC
    "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp_macos";
#elif defined(__aarch64__) || defined(__arm64__)
    "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp_linux_aarch64";
#else
    "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp_linux";
#endif

constexpr const char *kFfmpeg =
#if JUCE_WINDOWS
    "https://github.com/eugeneware/ffmpeg-static/releases/latest/download/ffmpeg-win32-x64";
#elif JUCE_MAC
    kArm ? "https://github.com/eugeneware/ffmpeg-static/releases/latest/download/ffmpeg-darwin-arm64"
         : "https://github.com/eugeneware/ffmpeg-static/releases/latest/download/ffmpeg-darwin-x64";
#else
    kArm ? "https://github.com/eugeneware/ffmpeg-static/releases/latest/download/ffmpeg-linux-arm64"
         : "https://github.com/eugeneware/ffmpeg-static/releases/latest/download/ffmpeg-linux-x64";
#endif

juce::String toolFileName(const juce::String &tool) {
#if JUCE_WINDOWS
  return tool + ".exe";
#else
  return tool;
#endif
}

// Descarga un binario crudo a la carpeta gestionada (sin zip ni tar).
bool downloadOne(const juce::String &tool,
                 const std::function<void(float)> &report,
                 const std::function<bool()> &alive) {
  const juce::File dir = ExternalTool::managedToolsDir();
  if (!dir.isDirectory() && !dir.createDirectory())
    return false;
  const juce::File dest = dir.getChildFile(toolFileName(tool));
  juce::URL u(tool == "yt-dlp" ? juce::String(kYtDlp) : juce::String(kFfmpeg));
  auto options =
      juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
          .withExtraHeaders("User-Agent: PerenkeGain-Tools\r\n"
                            "Accept: application/octet-stream")
          .withConnectionTimeoutMs(30000);
  auto in = u.createInputStream(options);
  if (in == nullptr)
    return false;

  // Primero a un .part y solo al final se mueve: no deja basura a medias.
  const juce::File tmp = dir.getChildFile("." + dest.getFileName() + ".part");
  tmp.deleteFile();
  if (!tmp.create().wasOk())
    return false;
  juce::FileOutputStream out(tmp);
  if (!out.openedOk())
    return false;

  constexpr int kChunk = 64 * 1024;
  const juce::int64 total = in->getTotalLength();
  juce::HeapBlock<char> buffer(kChunk);
  juce::int64 got = 0;
  int lastPercent = -1;
  bool failed = false;
  while (!in->isExhausted() && alive) {
    const int n = in->read(buffer.get(), kChunk);
    if (n <= 0)
      break;
    out.write(buffer, (size_t)n);
    if (out.getStatus().failed()) {
      failed = true;
      break;
    }
    got += n;
    if (total > 0 && report) {
      const int percent = (int)(got * 100 / total);
      if (percent != lastPercent) {
        lastPercent = percent;
        report((float)percent / 100.0f);
      }
    }
  }
  out.flush();
  if (!alive)
    return false;
  bool ok = !failed && got > 0 && (total <= 0 || got == total);
  if (!ok) {
    tmp.deleteFile();
    return false;
  }
  dest.deleteFile();
  if (!tmp.moveFileTo(dest))
    return false;
#if !JUCE_WINDOWS
  dest.setExecutePermission(true);
#endif
  if (report)
    report(1.0f);
  return true;
}

} // namespace

struct ToolInstaller::State : std::enable_shared_from_this<State> {
  juce::CriticalSection lock;
  bool alive = true;
  Progress progress;
  Done done;

  bool stillAlive() const {
    const juce::ScopedLock sl(lock);
    return alive;
  }
  void reportProgress(const juce::String &tool, float frac) {
    juce::MessageManager::callAsync([safe = shared_from_this(), tool, frac] {
      const juce::ScopedLock sl(safe->lock);
      if (safe->alive && safe->progress)
        safe->progress(tool, frac);
    });
  }
  void reportDone(bool ok, const juce::String &msg) {
    juce::MessageManager::callAsync([safe = shared_from_this(), ok, msg] {
      const juce::ScopedLock sl(safe->lock);
      if (safe->alive && safe->done)
        safe->done(ok, msg);
    });
  }
};

ToolInstaller::~ToolInstaller() { cancel(); }

void ToolInstaller::cancel() {
  if (state != nullptr) {
    const juce::ScopedLock sl(state->lock);
    state->alive = false;
    state->progress = nullptr;
    state->done = nullptr;
  }
}

juce::StringArray ToolInstaller::pending() {
  juce::StringArray todo;
  const juce::File ytdlp = ExternalTool::find("yt-dlp");
  if (ytdlp == juce::File{} || ExternalTool::isYtDlpOutdated(ytdlp))
    todo.add("yt-dlp");
  if (ExternalTool::find("ffmpeg") == juce::File{})
    todo.add("ffmpeg");
  return todo;
}

void ToolInstaller::install(Progress onProgress, Done onDone) {
  cancel();
  state = std::make_shared<State>();
  state->progress = std::move(onProgress);
  state->done = std::move(onDone);
  const juce::StringArray todo = pending();

  std::thread([state = state, todo] {
    if (todo.isEmpty()) {
      state->reportDone(true, {});
      return;
    }
    for (const auto &tool : todo) {
      if (!state->stillAlive())
        return;
      const bool ok = downloadOne(
          tool, [state, tool](float f) { state->reportProgress(tool, f); },
          [state] {
            const juce::ScopedLock sl(state->lock);
            return state->alive;
          });
      if (!state->stillAlive())
        return;
      if (!ok) {
        state->reportDone(
            false, PG_T("No se pudo descargar ") + tool +
                       PG_T(". Revisa tu conexión o instálalo a mano."));
        return;
      }
    }
    state->reportDone(true, {});
  }).detach();
}

} // namespace pg
