#include "ui/BatchTask.h"
#include "types/Text.h"

namespace pg {

BatchTask::BatchTask(std::vector<juce::File> f, WorkFn w, DoneFn d)
    : juce::ThreadWithProgressWindow(PG_T("Procesando audios..."), true, true),
      files(std::move(f)), work(std::move(w)), done(std::move(d)) {}

void BatchTask::run() {
  for (size_t i = 0; i < files.size(); ++i) {
    if (threadShouldExit())
      break;
    setStatusMessage(files[i].getFileName());
    if (work)
      work(files[i], (int)i);
    setProgress((double)(i + 1) / (double)files.size());
  }
}

void BatchTask::threadComplete(bool cancelled) {
  if (done)
    done(cancelled);
  delete this;
}

void BatchTask::analyze(
    std::vector<juce::File> files,
    std::function<void(std::vector<NormalizeAnalysis>)> onDone) {
  auto results = std::make_shared<std::vector<NormalizeAnalysis>>();
  auto *t = new BatchTask(
      std::move(files),
      [results](const juce::File &f, int) {
        results->push_back(BatchNormalizer::analyzeFile(f));
      },
      [results, onDone](bool) {
        if (onDone)
          onDone(std::move(*results));
      });
  t->launchThread();
}

void BatchTask::apply(std::vector<juce::File> files, std::vector<float> gains,
                      juce::File outDir,
                      std::function<void(juce::StringArray)> onDone) {
  auto gs = std::make_shared<std::vector<float>>(std::move(gains));
  auto results = std::make_shared<juce::StringArray>();
  auto *t = new BatchTask(
      std::move(files),
      [gs, outDir, results](const juce::File &f, int i) {
        juce::String err;
        const bool ok =
            BatchNormalizer::writeNormalized(f, (*gs)[(size_t)i], outDir, err);
        results->add(f.getFileName() + "  " +
                     (ok ? juce::String("OK") : PG_T("ERROR: ") + err));
      },
      [results, onDone](bool) {
        if (onDone)
          onDone(*results);
      });
  t->launchThread();
}

} // namespace pg
