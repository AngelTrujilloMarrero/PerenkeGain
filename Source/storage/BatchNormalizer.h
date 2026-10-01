#pragma once
#include <JuceHeader.h>
#include <vector>

namespace pg {

// Medida de un fichero (fase de analisis, tipo mp3gain). La ganancia se
// calcula aparte a partir del objetivo.
struct NormalizeAnalysis {
  juce::File file;
  float measuredDb = 0.0f; // volumen medido (escala mp3gain, 89 dB = -14 dBFS)
  float peakDb = -100.0f;  // pico en dBFS
  bool ok = false;
};

// Normalizacion offline por sonoridad percibida (tipo mp3gain/ReplayGain) en
// dos fases: analizar y luego escribir aplicando la ganancia, conservando el
// formato de entrada (WAV/AIFF/FLAC/OGG; MP3 con lame/ffmpeg externos).
class BatchNormalizer {
public:
  static constexpr float kReferenceDb = 89.0f;
  static constexpr float kReferenceDbFs = -14.0f;

  // Fichero suelto o todos los audios de una carpeta (recursivo).
  static std::vector<juce::File> collectFiles(const juce::File &source,
                                              bool recursive = true);
  static NormalizeAnalysis analyzeFile(const juce::File &in);
  // Aplica gainDb y guarda en outDir con el mismo formato de entrada.
  static bool writeNormalized(const juce::File &in, float gainDb,
                              const juce::File &outDir, juce::String &error);
  // Ganancia unica de album (energia combinada) para el objetivo dado.
  static float albumGainDb(const std::vector<NormalizeAnalysis> &analyses,
                           float targetDb);
};

} // namespace pg
