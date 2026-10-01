#pragma once
#include <JuceHeader.h>

namespace pg {

// Aviso elegante en la parte superior: "hay una version nueva".
// Todo el banner es pulsable (abre el detalle) salvo la "x" de cerrar.
class UpdateBannerComponent : public juce::Component {
public:
  UpdateBannerComponent();

  void paint(juce::Graphics &g) override;
  void resized() override;
  void mouseUp(const juce::MouseEvent &e) override;
  void mouseEnter(const juce::MouseEvent &) override;
  void mouseExit(const juce::MouseEvent &) override;

  void setVersion(const juce::String &latest, const juce::String &current);

  std::function<void()> onShowDetails;
  std::function<void()> onDismiss;

private:
  juce::ShapeButton closeBtn;
  juce::String latestVersion, currentVersion;
  bool hover = false;
};

} // namespace pg
