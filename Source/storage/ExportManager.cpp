#include "storage/ExportManager.h"

namespace pg {

bool ExportManager::exportTracks(const juce::AudioBuffer<float> &buf,
                                 double sr,
                                 const std::vector<TrackRegion> &regions,
                                 const juce::File &dir, int formatId) {
  // Writers reales JUCE. MP3 no tiene encoder en JUCE (solo lector):
  // se exporta como WAV 32-float con extensión .wav (LAME/ffmpeg = TODO).
  juce::WavAudioFormat wavFormat;
  juce::FlacAudioFormat flacFormat;
  juce::AudioFormat *fmt = &wavFormat;
  juce::String ext = ".wav";
  if (formatId == 3) {
    fmt = &flacFormat;
    ext = ".flac";
  }

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
      std::unique_ptr<juce::AudioFormatWriter> w(fmt->createWriterFor(
          os.get(), sr, (unsigned int)clip.getNumChannels(), 32,
          juce::StringPairArray(), 0));
      if (w != nullptr) {
        w->writeFromAudioSampleBuffer(clip, 0, len);
        os.release();
      }
    }
  }
  return true;
}

} // namespace pg
