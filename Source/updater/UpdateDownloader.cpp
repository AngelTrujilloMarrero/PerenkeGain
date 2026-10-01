#include "updater/UpdateDownloader.h"
#include <thread>

namespace pg::updater {

struct UpdateDownloader::State : std::enable_shared_from_this<State> {
  juce::CriticalSection lock;
  bool alive = true;
  Progress progress;
  Finished finished;

  void reportProgress(double value) {
    juce::MessageManager::callAsync([safe = shared_from_this(), value] {
      const juce::ScopedLock sl(safe->lock);
      if (safe->alive && safe->progress)
        safe->progress(value);
    });
  }

  void reportFinished(bool ok, juce::File file) {
    juce::MessageManager::callAsync([safe = shared_from_this(), ok, file] {
      const juce::ScopedLock sl(safe->lock);
      if (safe->alive && safe->finished)
        safe->finished(ok, file);
    });
  }
};

UpdateDownloader::UpdateDownloader() = default;

UpdateDownloader::~UpdateDownloader() { cancel(); }

void UpdateDownloader::cancel() {
  if (state != nullptr) {
    const juce::ScopedLock sl(state->lock);
    state->alive = false;
    state->progress = nullptr;
    state->finished = nullptr;
  }
}

void UpdateDownloader::start(const UpdateAsset &asset, juce::File target,
                             Progress onProgress, Finished onFinished) {
  cancel();

  state = std::make_shared<State>();
  state->progress = std::move(onProgress);
  state->finished = std::move(onFinished);

  const juce::String url = asset.downloadUrl;
  const juce::int64 expectedSize = asset.sizeBytes;
  std::thread([state = state, url, target, expectedSize] {
    juce::URL u(url);
    auto options =
        juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
            .withExtraHeaders("User-Agent: PerenkeGain-Updater\r\n"
                              "Accept: application/octet-stream")
            .withConnectionTimeoutMs(30000);
    auto in = u.createInputStream(options);
    if (in == nullptr) {
      state->reportFinished(false, target);
      return;
    }

    target.deleteFile();
    if (!target.create().wasOk()) {
      state->reportFinished(false, target);
      return;
    }

    juce::FileOutputStream out(target);
    if (!out.openedOk()) {
      state->reportFinished(false, target);
      return;
    }

    constexpr int kChunkSize = 64 * 1024;
    const juce::int64 total = in->getTotalLength();
    juce::HeapBlock<char> buffer(kChunkSize);
    juce::int64 downloaded = 0;
    int lastPercent = -1;
    bool writeError = false;
    while (!in->isExhausted()) {
      const int n = in->read(buffer.get(), kChunkSize);
      if (n <= 0)
        break;
      out.write(buffer, (size_t)n);
      if (out.getStatus().failed()) {
        writeError = true;
        break;
      }
      downloaded += n;
      if (total > 0) {
        const int percent = (int)(downloaded * 100 / total);
        if (percent != lastPercent) {
          lastPercent = percent;
          state->reportProgress((double)percent / 100.0);
        }
      }
    }
    out.flush();

    // Una descarga truncada (corte de red, disco lleno, pagina de error
    // HTTP guardada como fichero) no es una descarga valida: antes
    // cualquier fichero de mas de 0 bytes contaba como exito y el
    // instalador intentaba instalar basura.
    bool ok = !writeError && downloaded > 0;
    if (ok && total > 0 && downloaded != total)
      ok = false;
    if (ok && expectedSize > 0 && downloaded < expectedSize)
      ok = false;
    if (!ok)
      target.deleteFile();
    state->reportFinished(ok, target);
  }).detach();
}

} // namespace pg::updater
