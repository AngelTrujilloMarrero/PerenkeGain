#include "storage/BatchNormalizer.h"
#include "storage/AudioFileLoader.h"
#include "storage/AudioEncoder.h"
#include "dsp/LoudnessAnalyzer.h"
#include <algorithm>
#include <cmath>

namespace pg {

std::vector<juce::File> BatchNormalizer::collectFiles(const juce::File &source,
                                                      bool recursive) {
  std::vector<juce::File> out;
  if (source.isDirectory()) {
    auto files = source.findChildFiles(
        juce::File::findFiles, recursive,
        "*.wav;*.wave;*.mp3;*.flac;*.ogg;*.aiff;*.aif");
    out.assign(files.begin(), files.end());
  } else if (source.existsAsFile()) {
    out.push_back(source);
  }
  std::sort(out.begin(), out.end(), [](const juce::File &a, const juce::File &b) {
    return a.getFullPathName() < b.getFullPathName();
  });
  return out;
}

NormalizeAnalysis BatchNormalizer::analyzeFile(const juce::File &in) {
  NormalizeAnalysis a;
  a.file = in;

  juce::AudioBuffer<float> buf;
  double sr = 44100.0;
  if (!AudioFileLoader::load(in, buf, sr))
    return a;

  const float rmsDb = LoudnessAnalyzer::rmsDbF(buf, sr);
  a.measuredDb = rmsDb + (kReferenceDb - kReferenceDbFs);

  float peak = 0.0f;
  for (int ch = 0; ch < buf.getNumChannels(); ++ch) {
    const auto *d = buf.getReadPointer(ch);
    for (int i = 0; i < buf.getNumSamples(); ++i)
      peak = juce::jmax(peak, std::abs(d[i]));
  }
  a.peakDb = peak > 0.0f ? juce::Decibels::gainToDecibels(peak, -100.0f)
                         : -100.0f;
  a.ok = true;
  return a;
}

bool BatchNormalizer::writeNormalized(const juce::File &in, float gainDb,
                                      const juce::File &outDir,
                                      juce::String &error) {
  juce::AudioBuffer<float> buf;
  double sr = 44100.0;
  if (!AudioFileLoader::load(in, buf, sr)) {
    error = "No se pudo leer";
    return false;
  }
  buf.applyGain(juce::Decibels::decibelsToGain(gainDb));

  const juce::String ext = in.getFileExtension().toLowerCase();
  const juce::File out =
      outDir.getChildFile(in.getFileNameWithoutExtension() + "_norm" + ext);
  return AudioEncoder::write(out, buf, sr, ext, error);
}

float BatchNormalizer::albumGainDb(
    const std::vector<NormalizeAnalysis> &analyses, float targetDb) {
  double meanSquare = 0.0;
  bool any = false;
  for (const auto &a : analyses) {
    if (!a.ok)
      continue;
    const double rmsDb = a.measuredDb - (kReferenceDb - kReferenceDbFs);
    meanSquare += std::pow(10.0, rmsDb / 10.0);
    any = true;
  }
  if (!any || meanSquare <= 0.0)
    return 0.0f;
  const double albumRms = 10.0 * std::log10(meanSquare);
  const double albumMeasured = albumRms + (kReferenceDb - kReferenceDbFs);
  return targetDb - (float)albumMeasured;
}

} // namespace pg
