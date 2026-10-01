#include "ui/NormalizeReport.h"
#include "types/Text.h"

namespace pg {

NormalizePlan planNormalization(const std::vector<NormalizeAnalysis> &analyses,
                                float targetDb, bool preventClip) {
  NormalizePlan plan;
  plan.trackGains.assign(analyses.size(), 0.0f);
  float minNoClip = 1.0e9f;
  for (size_t i = 0; i < analyses.size(); ++i) {
    const auto &a = analyses[i];
    if (!a.ok)
      continue;
    float g = targetDb - a.measuredDb;
    if (preventClip)
      g = juce::jmin(g, -a.peakDb);
    plan.trackGains[i] = g;
    minNoClip = juce::jmin(minNoClip, -a.peakDb);
  }
  plan.albumGainDb = BatchNormalizer::albumGainDb(analyses, targetDb);
  if (preventClip && minNoClip < 1.0e8f)
    plan.albumGainDb = juce::jmin(plan.albumGainDb, minNoClip);
  return plan;
}

juce::String formatNormalizeReport(
    const std::vector<NormalizeAnalysis> &analyses, const NormalizePlan &plan,
    const juce::String &title) {
  juce::String text;
  if (title.isNotEmpty())
    text << title << "\n";
  text << PG_T("Fichero") << "                  " << PG_T("Volumen") << "   "
       << PG_T("Pico") << "      " << PG_T("Ganancia") << "\n";
  for (size_t i = 0; i < analyses.size(); ++i) {
    const auto &a = analyses[i];
    text << a.file.getFileName().paddedRight(' ', 24);
    if (!a.ok) {
      text << PG_T("  (no se pudo leer)\n");
      continue;
    }
    const float g = plan.trackGains[i];
    text << juce::String(a.measuredDb, 1).paddedLeft(' ', 7) << " "
         << juce::String(a.peakDb, 1).paddedLeft(' ', 8) << "  "
         << ((g > 0.0f) ? "+" : "") << juce::String(g, 1).paddedLeft(' ', 6)
         << " dB";
    if (a.peakDb + g > 0.0f)
      text << PG_T("  (recorte)");
    text << "\n";
  }
  text << "\n"
       << PG_T("Ganancia de álbum: ")
       << ((plan.albumGainDb > 0.0f) ? "+" : "")
       << juce::String(plan.albumGainDb, 1) << " dB\n";
  return text;
}

} // namespace pg
