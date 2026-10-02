#include "ui/ModernLookAndFeel.h"

namespace pg::modern {

juce::Colour surface() { return juce::Colour(0xFF14151A); }
juce::Colour card() { return juce::Colour(0xFF1E2028); }
juce::Colour accent() { return juce::Colour(0xFF00E0E0); }

void fillBackground(juce::Graphics &g, juce::Rectangle<int> r) {
  g.setColour(surface());
  g.fillAll();
  juce::ignoreUnused(r);
}

void frame(juce::Graphics &g, juce::Rectangle<int> r) {
  g.setColour(juce::Colour(0xFF3A4152));
  g.drawRoundedRectangle(r.toFloat().reduced(0.5f), 4.0f, 1.0f);
}

} // namespace pg::modern

namespace pg {

ModernLookAndFeel::ModernLookAndFeel() {
  const auto text = juce::Colour(0xFFF2F3F6);
  const auto muted = juce::Colour(0xFF9AA0AC);
  setColour(juce::ResizableWindow::backgroundColourId,
            modern::surface());
  setColour(juce::DocumentWindow::backgroundColourId, modern::surface());
  setColour(juce::Label::textColourId, text);
  setColour(juce::TextButton::textColourOffId, text);
  setColour(juce::TextButton::textColourOnId, juce::Colours::black);
  setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF262B36));
  setColour(juce::TextButton::buttonOnColourId, modern::accent());
  setColour(juce::ToggleButton::textColourId, text);
  setColour(juce::ToggleButton::tickColourId, modern::accent());
  setColour(juce::ToggleButton::tickDisabledColourId,
            juce::Colour(0xFF555B66));
  setColour(juce::ComboBox::textColourId, text);
  setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF262B36));
  setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF3A4152));
  setColour(juce::ComboBox::arrowColourId, muted);
  setColour(juce::TextEditor::textColourId, text);
  setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF0E1014));
  setColour(juce::TextEditor::outlineColourId, juce::Colour(0xFF3A4152));
  setColour(juce::ListBox::backgroundColourId, juce::Colour(0xFF0E1014));
  setColour(juce::ListBox::textColourId, text);
  setColour(juce::Slider::textBoxTextColourId, text);
  setColour(juce::Slider::textBoxBackgroundColourId,
            juce::Colour(0xFF14151A));
  setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xFF3A4152));
  setColour(juce::Slider::trackColourId, juce::Colour(0xFF2A2E38));
  setColour(juce::Slider::thumbColourId, modern::accent());
  setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xFF1E2028));
  setColour(juce::PopupMenu::textColourId, text);
  setColour(juce::PopupMenu::highlightedBackgroundColourId,
            juce::Colour(0xFF2B3A55));
  setColour(juce::ScrollBar::thumbColourId, juce::Colour(0xFF3A4152));
  setColour(juce::ScrollBar::trackColourId, juce::Colour(0xFF14151A));
}

void ModernLookAndFeel::drawButtonBackground(juce::Graphics &g,
                                             juce::Button &b,
                                             const juce::Colour &, bool hover,
                                             bool down) {
  auto r = b.getLocalBounds().toFloat().reduced(0.5f);
  const bool on =
      b.getToggleState() && b.getClickingTogglesState();
  juce::Colour fill = juce::Colour(0xFF262B36);
  if (!b.isEnabled())
    fill = fill.withAlpha(0.4f);
  else if (on || down)
    fill = modern::accent().darker(0.15f);
  else if (hover)
    fill = juce::Colour(0xFF323949);
  g.setColour(fill);
  g.fillRoundedRectangle(r, 6.0f);
  g.setColour(on || down ? modern::accent().brighter(0.3f)
                         : juce::Colour(0xFF3A4152));
  g.drawRoundedRectangle(r, 6.0f, 1.0f);
}

void ModernLookAndFeel::drawToggleButton(juce::Graphics &g,
                                         juce::ToggleButton &b, bool hover,
                                         bool) {
  auto r = b.getLocalBounds();
  juce::Rectangle<float> box(r.getX() + 3.0f, r.getCentreY() - 8.0f, 16.0f,
                             16.0f);
  g.setColour(b.getToggleState() ? modern::accent().darker(0.15f)
                                 : juce::Colour(0xFF262B36));
  g.fillRoundedRectangle(box, 4.0f);
  g.setColour(b.getToggleState() ? modern::accent().brighter(0.3f)
              : hover            ? juce::Colour(0xFF4A5265)
                                 : juce::Colour(0xFF3A4152));
  g.drawRoundedRectangle(box, 4.0f, 1.2f);
  if (b.getToggleState()) {
    g.setColour(b.isEnabled() ? juce::Colours::black
                               : juce::Colour(0xFF555B66));
    juce::Path tick;
    tick.addTriangle(box.getX() + 3.5f, box.getCentreY() - 0.5f,
                     box.getX() + 7.0f, box.getBottom() - 4.0f,
                     box.getRight() - 3.0f, box.getY() + 4.5f);
    tick.addTriangle(box.getX() + 3.5f, box.getCentreY() + 1.5f,
                     box.getX() + 7.0f, box.getBottom() - 2.0f,
                     box.getRight() - 3.0f, box.getY() + 6.5f);
    g.fillPath(tick);
  }
  g.setColour(b.findColour(juce::ToggleButton::textColourId));
  g.drawText(b.getButtonText(),
             r.withTrimmedLeft(24).toFloat(),
             juce::Justification::centredLeft);
}

void ModernLookAndFeel::drawComboBox(juce::Graphics &g, int width, int height,
                                     bool down, int, int, int, int,
                                     juce::ComboBox &box) {
  auto r = juce::Rectangle<float>(0, 0, (float)width, (float)height);
  g.setColour(box.isEnabled() ? juce::Colour(0xFF262B36)
                              : juce::Colour(0xFF1A1D24));
  g.fillRoundedRectangle(r.reduced(0.5f), 6.0f);
  g.setColour(down ? modern::accent() : juce::Colour(0xFF3A4152));
  g.drawRoundedRectangle(r.reduced(0.5f), 6.0f, 1.0f);
  const float ax = r.getWidth() - 18.0f, ay = r.getCentreY();
  juce::Path arrow;
  arrow.addTriangle(ax, ay - 3.5f, ax + 9.0f, ay - 3.5f, ax + 4.5f, ay + 3.0f);
  g.setColour(box.findColour(juce::ComboBox::arrowColourId));
  g.fillPath(arrow);
}

void ModernLookAndFeel::drawLinearSlider(
    juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
    float, float, const juce::Slider::SliderStyle style, juce::Slider &) {
  auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width,
                                       (float)height);
  const bool vertical = (style == juce::Slider::LinearVertical);
  juce::Rectangle<float> track, fill;
  if (vertical) {
    track = {bounds.getCentreX() - 2.5f, bounds.getY() + 4.0f, 5.0f,
             bounds.getHeight() - 8.0f};
    fill = {track.getX(), sliderPos, track.getWidth(),
            track.getBottom() - sliderPos};
  } else {
    track = {bounds.getX() + 4.0f, bounds.getCentreY() - 2.5f,
             bounds.getWidth() - 8.0f, 5.0f};
    fill = {track.getX(), track.getY(), sliderPos - track.getX(),
            track.getHeight()};
  }
  g.setColour(juce::Colour(0xFF2A2E38));
  g.fillRoundedRectangle(track, 2.5f);
  g.setColour(modern::accent().darker(0.1f));
  g.fillRoundedRectangle(fill.getIntersection(track), 2.5f);
  juce::Rectangle<float> thumb =
      vertical ? juce::Rectangle<float>(bounds.getX() + 1.0f, sliderPos - 7.0f,
                                        bounds.getWidth() - 2.0f, 14.0f)
               : juce::Rectangle<float>(sliderPos - 7.0f, bounds.getY() + 1.0f,
                                        14.0f, bounds.getHeight() - 2.0f);
  g.setColour(juce::Colour(0xFFE9EDF4));
  g.fillRoundedRectangle(thumb, 4.0f);
  g.setColour(juce::Colour(0xFF3A4152));
  g.drawRoundedRectangle(thumb.reduced(0.5f), 4.0f, 1.0f);
}

} // namespace pg
