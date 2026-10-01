#pragma once
#include <JuceHeader.h>
#include <functional>
#include "storage/BatchNormalizer.h"

namespace pg {

// Hilo generico con barra de progreso y cancelar: recorre ficheros llamando a
// 'work' y avisa al terminar por 'done' en el hilo de mensajes. Se
// autodestruye (patron de ThreadWithProgressWindow con launchThread).
class BatchTask : public juce::ThreadWithProgressWindow {
public:
  using WorkFn = std::function<void(const juce::File &, int index)>;
  using DoneFn = std::function<void(bool cancelled)>;

  BatchTask(std::vector<juce::File> files, WorkFn work, DoneFn done);
  void run() override;
  void threadComplete(bool userPressedCancel) override;

  // Atajos: analizan o aplican ganancias en segundo plano.
  static void analyze(std::vector<juce::File> files,
                      std::function<void(std::vector<NormalizeAnalysis>)> onDone);
  static void apply(std::vector<juce::File> files, std::vector<float> gains,
                    juce::File outDir,
                    std::function<void(juce::StringArray)> onDone);

private:
  std::vector<juce::File> files;
  WorkFn work;
  DoneFn done;
};

} // namespace pg
