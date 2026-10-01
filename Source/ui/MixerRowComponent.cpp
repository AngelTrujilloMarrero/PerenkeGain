#include "ui/MixerRowComponent.h"
#include "types/EqTypes.h"

namespace pg {

MixerRowComponent::MixerRowComponent() {
  for (size_t i = 0; i < 31; ++i) {
    strips[i] = std::make_unique<MixerStripComponent>((int)i);
    strips[i]->onEdit = [this, b = (int)i] {
      if (onReadout)
        onReadout(infoFor(b));
    };
    addAndMakeVisible(*strips[i]);
  }
  startTimerHz(30);
}

void MixerRowComponent::resized() {
  auto r = getLocalBounds();
  int w = r.getWidth() / 31;
  for (auto &s : strips)
    s->setBounds(r.removeFromLeft(w));
}

float MixerRowComponent::gainDb(int i) const { return strips[(size_t)i]->gainDb(); }

void MixerRowComponent::setGainDb(int i, float db) {
  strips[(size_t)i]->setGainDb(db);
}

bool MixerRowComponent::isMuted(int i) const {
  return strips[(size_t)i]->isMuted();
}

void MixerRowComponent::setMuted(int i, bool muted) {
  strips[(size_t)i]->setMuted(muted);
}

void MixerRowComponent::timerCallback() {
  if (!isShowing() || analyzer == nullptr)
    return;
  for (int b = 0; b < BandLevelAnalyzer::kBands; ++b)
    // Banda muteada: medidor congelado a suelo (no "sigue funcionando").
    strips[(size_t)b]->setLevelDb(strips[(size_t)b]->isMuted()
                                      ? -60.0f
                                      : analyzer->levelDb(b));
}

juce::String MixerRowComponent::infoFor(int i) const {
  float f = kEq31Freqs[(size_t)i];
  juce::String t = f >= 1000.f
                       ? juce::String(juce::roundToInt(f / 1000.f)) + " kHz"
                       : juce::String(juce::roundToInt(f)) + " Hz";
  juce::String g = (strips[(size_t)i]->gainDb() >= 0 ? "+" : "") +
                   juce::String(strips[(size_t)i]->gainDb(), 1) + " dB";
  return t + "   " + g + "   " +
         (strips[(size_t)i]->isMuted() ? "MUTE" : "ON");
}

} // namespace pg
