#include "storage/ExportManager.h"

namespace pg {

bool ExportManager::exportTracks(const juce::AudioBuffer<float> &buf, double sr,
                                 const std::vector<TrackRegion> &regions,
                                 const juce::File &dir, int formatId) {
  // Writers reales JUCE. MP3 no tiene encoder en JUCE (solo lector): se
  // exporta como WAV 32-float con extensión .wav (LAME/ffmpeg = TODO).
  juce::WavAudioFormat wavFormat;
  juce::FlacAudioFormat flacFormat;
  juce::AudioFormat *fmt = &wavFormat;
  juce::String ext = ".wav";
  int bits = 32;
  auto sampleFormat =
      juce::AudioFormatWriterOptions::SampleFormat::floatingPoint;
  if (formatId == 3) {
    fmt = &flacFormat;
    ext = ".flac";
    bits = 24;
    sampleFormat = juce::AudioFormatWriterOptions::SampleFormat::integral;
  }

  const int total = buf.getNumSamples();
  bool ok = true;
  for (const auto &tr : regions) {
    int s0 = juce::jlimit(0, total, (int)(tr.startSec * sr));
    int s1 = juce::jlimit(0, total, (int)(tr.endSec * sr));
    int len = juce::jlimit(1, juce::jmax(1, total - s0), s1 - s0);
    if (s0 >= total)
      continue;

    juce::AudioBuffer<float> clip(buf.getNumChannels(), len);
    clip.clear();
    for (int ch = 0; ch < buf.getNumChannels(); ++ch)
      clip.copyFrom(ch, 0, buf, ch, s0, len);

    juce::File out =
        dir.getChildFile("pista_" + juce::String(tr.index + 1) + ext);
    std::unique_ptr<juce::OutputStream> os = out.createOutputStream();
    if (os == nullptr) {
      ok = false;
      continue;
    }
    auto options = juce::AudioFormatWriterOptions{}
                       .withSampleRate(sr)
                       .withNumChannels(clip.getNumChannels())
                       .withBitsPerSample(bits)
                       .withSampleFormat(sampleFormat);
    if (auto w = fmt->createWriterFor(os, options))
      w->writeFromAudioSampleBuffer(clip, 0, len);
    else
      ok = false;
  }
  return ok;
}

} // namespace pg
