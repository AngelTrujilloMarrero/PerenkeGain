#pragma once
#include <JuceHeader.h>

namespace pg {

// Tira de canal de una banda (mesa digital moderna):
// medidor LED + fader de ganancia con capuchón + botón MUTE.
class MixerStripComponent : public juce::Component {
public:
  explicit MixerStripComponent(int bandIndex);
  void paint(juce::Graphics &g) override;
  void mouseDown(const juce::MouseEvent &e) override;
  void mouseDrag(const juce::MouseEvent &e) override;
  void mouseUp(const juce::MouseEvent &e) override;

  void setLevelDb(float db); // llamado por la fila a 30 Hz
  float gainDb() const { return gain; }
  void setGainDb(float db);
  bool isMuted() const { return muted; }
  void setMuted(bool m);

  // Avisa a la fila para actualizar el lector (banda/gain/mute).
  std::function<void()> onEdit;

private:
  int band;
  float gain = 0.0f;   // -12..+12 dB
  float level = -60.0f; // -60..0 dB
  bool muted = false;
  bool dragging = false;
  float dragStartY = 0.0f, dragStartGain = 0.0f;

  juce::Rectangle<int> meterArea() const;
  juce::Rectangle<int> faderArea() const;
  juce::Rectangle<int> muteArea() const;
  void paintMeter(juce::Graphics &g);
  void paintFader(juce::Graphics &g);
  void paintMute(juce::Graphics &g);
};

} // namespace pg
