#pragma once
#include "updater/UpdateInfo.h"
#include <functional>
#include <memory>

namespace pg::updater {

// Descarga un asset de la release en un hilo de fondo.
// progress(0..1) y finished(ok, fichero) siempre llegan en el hilo de
// mensajes. Es seguro destruir el downloader mientras descarga.
class UpdateDownloader {
public:
  using Progress = std::function<void(double)>;
  using Finished = std::function<void(bool, juce::File)>;

  UpdateDownloader();
  ~UpdateDownloader();

  void start(const UpdateAsset &asset, juce::File target, Progress onProgress,
             Finished onFinished);
  void cancel();

private:
  struct State;
  std::shared_ptr<State> state;
};

} // namespace pg::updater
