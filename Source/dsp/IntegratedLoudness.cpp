#include "dsp/IntegratedLoudness.h"
#include "dsp/KWeightingFilter.h"
#include <cmath>
#include <vector>

namespace pg {

float IntegratedLoudness::analyze(const juce::AudioBuffer<float> &buf,
                                  double sampleRate) {
  const int nch = buf.getNumChannels();
  const int n = buf.getNumSamples();
  if (nch <= 0 || n <= 0 || sampleRate <= 0.0)
    return -100.0f;

  // Un filtro K por canal (KWeightingFilter solo soporta 2, usamos uno mono
  // por canal para no limitar el numero de canales).
  std::vector<KWeightingFilter> filters((size_t)nch);
  for (int ch = 0; ch < nch; ++ch)
    filters[(size_t)ch].prepare(sampleRate, 1);

  // Energia K-weighted por canal, acumulada por hops de 100 ms.
  const int hopSamples = juce::jmax(1, (int)std::round(sampleRate * 0.1));
  const int numHops = (n + hopSamples - 1) / hopSamples;
  std::vector<std::vector<double>> hopEnergy(
      (size_t)nch, std::vector<double>((size_t)numHops, 0.0));

  for (int ch = 0; ch < nch; ++ch) {
    const float *d = buf.getReadPointer(ch);
    auto &kf = filters[(size_t)ch];
    for (int hop = 0; hop < numHops; ++hop) {
      const int start = hop * hopSamples;
      const int len = juce::jmin(hopSamples, n - start);
      double sumSq = 0.0;
      for (int i = 0; i < len; ++i) {
        const float y = kf.processSample(0, d[start + i]);
        sumSq += (double)y * (double)y;
      }
      hopEnergy[(size_t)ch][(size_t)hop] = sumSq / (double)len;
    }
  }

  // Ventanas de 400 ms = 4 hops (o menos si el fichero es corto).
  const int winHops = 4;
  std::vector<double> blockEnergy;
  blockEnergy.reserve((size_t)numHops);
  for (int hop = 0; hop < numHops; ++hop) {
    const int count = juce::jmin(winHops, numHops - hop);
    if (count <= 0)
      continue;
    // Solo ventanas completas, salvo ficheros < 400 ms (una ventana).
    if (numHops >= winHops && count < winHops)
      break;
    double sum = 0.0;
    for (int ch = 0; ch < nch; ++ch)
      for (int h = 0; h < count; ++h)
        sum += hopEnergy[(size_t)ch][(size_t)(hop + h)];
    blockEnergy.push_back(sum / (double)count);
  }
  if (blockEnergy.empty())
    return -100.0f;

  constexpr double kOffset = -0.691; // calibracion BS.1770
  auto toLufs = [](double e) { return kOffset + 10.0 * std::log10(e); };

  // Gate absoluto -70 LUFS.
  std::vector<double> absPassed;
  for (double e : blockEnergy)
    if (e > 1.0e-12 && toLufs(e) > -70.0)
      absPassed.push_back(e);
  if (absPassed.empty())
    return -100.0f;

  // Primer integrado para el gate relativo (-10 LU).
  double mean = 0.0;
  for (double e : absPassed)
    mean += e;
  mean /= (double)absPassed.size();
  const double relGate = toLufs(mean) - 10.0;

  double gated = 0.0;
  int gatedCount = 0;
  for (double e : absPassed)
    if (toLufs(e) > relGate) {
      gated += e;
      ++gatedCount;
    }
  if (gatedCount == 0)
    return -100.0f;
  return (float)toLufs(gated / (double)gatedCount);
}

} // namespace pg
