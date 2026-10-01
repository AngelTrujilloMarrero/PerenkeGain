#include "ui/YouTubeTask.h"
#include "storage/BatchNormalizer.h"
#include "types/Text.h"

namespace pg {

YouTubeTask::YouTubeTask(std::vector<juce::String> u, juce::File dir,
                         float targetDb, DoneFn done)
    : juce::Thread("pg-youtube"), urls(std::move(u)), outDir(std::move(dir)),
      target(targetDb), onDone(std::move(done)) {}

void YouTubeTask::run() {
  const int n = (int)urls.size();
  for (int i = 0; i < n; ++i) {
    if (threadShouldExit())
      break;
    auto res = YouTubeDownloader::download(urls[(size_t)i], outDir, {});
    if (res.ok) {
      normalizeInPlace(res);
      okFiles.add(res.file);
    } else {
      errors.add(res.error);
    }
  }
  auto cb = onDone;
  auto ok = okFiles;
  auto err = errors;
  juce::MessageManager::callAsync(
      [cb, ok, err]() mutable {
        if (cb)
          cb(ok, err);
      });
}

void YouTubeTask::normalizeInPlace(YouTubeResult &res) {
  const auto a = BatchNormalizer::analyzeFile(res.file);
  if (!a.ok)
    return;
  float gain = target - a.measuredDb;
  gain = juce::jmin(gain, -a.peakDb); // evita recorte
  juce::String err;
  if (!BatchNormalizer::writeNormalized(res.file, gain, outDir, err))
    return;
  juce::File norm = outDir.getChildFile(res.file.getFileNameWithoutExtension() +
                                        "_norm" + res.file.getFileExtension());
  if (norm.existsAsFile()) {
    res.file.deleteFile();
    if (norm.moveFileTo(res.file))
      return;
    res.file = norm; // si no se pudo mover, dejamos el _norm
  }
}

} // namespace pg
