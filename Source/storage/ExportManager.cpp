#include "storage/ExportManager.h"

namespace pg {

bool ExportManager::exportTracks(const juce::AudioBuffer<float> &buf,
                                 double sr,
                                 const std::vector<TrackRegion> &regions,
                                 const juce::File &dir, int formatId) {
  juce::AudioFormatManager fm;
  fm.registerBasicFormats();
  std::unique_ptr<juce::AudioFormat> fmt;
  juce::String ext = ".wav";
  if (formatId == 3)
    ext = ".flac";
  else if (formatId == 2)
    ext = ".mp3";

  for (auto &tr : regions) {
    int s0 = int(tr.startSec * sr);
    int s1 = int(tr.endSec * sr);
    int len = juce::jmax(1, s1 - s0);
    juce::AudioBuffer<float> clip(buf.getNumChannels(), len);
    for (int ch = 0; ch < buf.getNumChannels(); ++ch)
      clip.copyFrom(ch, 0, buf, ch, s0, len);
    juce::File out =
        dir.getChildFile("pista_" + juce::String(tr.index + 1) + ext);
    if (auto os = out.createOutputStream()) {
      if (auto *w = fm.getDefaultFormatWriter(
              clip, (int)sr, *os, ext == ".mp3" ? nullptr : nullptr))
        delete w;
    }
  }
  return true;
}

} // namespace pg
