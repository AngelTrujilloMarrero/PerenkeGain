#include "ui/RetroLookAndFeel.h"

namespace pg::retro {

juce::Colour face() { return juce::Colour(0xFFD4D0C8); }

void bevelRaised(juce::Graphics &g, juce::Rectangle<int> r) {
  g.setColour(juce::Colours::white);
  g.drawHorizontalLine(r.getY(), (float)r.getX(), r.getRight() - 1.0f);
  g.drawVerticalLine(r.getX(), (float)r.getY(), r.getBottom() - 1.0f);
  g.setColour(juce::Colour(0xFF404040));
  g.drawHorizontalLine(r.getBottom() - 1, (float)r.getX(),
                       (float)r.getRight());
  g.drawVerticalLine(r.getRight() - 1, (float)r.getY(), (float)r.getBottom());
}

void bevelSunken(juce::Graphics &g, juce::Rectangle<int> r) {
  g.setColour(juce::Colour(0xFF808080));
  g.drawHorizontalLine(r.getY(), (float)r.getX(), r.getRight() - 1.0f);
  g.drawVerticalLine(r.getX(), (float)r.getY(), r.getBottom() - 1.0f);
  g.setColour(juce::Colours::white);
  g.drawHorizontalLine(r.getBottom() - 1, (float)r.getX(),
                       (float)r.getRight());
  g.drawVerticalLine(r.getRight() - 1, (float)r.getY(), (float)r.getBottom());
}

} // namespace pg::retro

namespace pg {

RetroLookAndFeel::RetroLookAndFeel() {
  setColour(juce::ResizableWindow::backgroundColourId, retro::face());
  setColour(juce::Label::textColourId, juce::Colours::black);
  setColour(juce::TextButton::textColourOffId, juce::Colours::black);
  setColour(juce::ToggleButton::textColourId, juce::Colours::black);
  setColour(juce::ComboBox::textColourId, juce::Colours::black);
  setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
  setColour(juce::TextEditor::textColourId, juce::Colours::black);
  setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
  setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
  setColour(juce::Slider::textBoxOutlineColourId, retro::face());
  setColour(juce::PopupMenu::backgroundColourId, retro::face());
  setColour(juce::PopupMenu::textColourId, juce::Colours::black);
  setColour(juce::ScrollBar::thumbColourId, juce::Colour(0xFFC0C0C0));
  setColour(juce::ScrollBar::trackColourId, juce::Colours::white);
}

void RetroLookAndFeel::drawButtonBackground(juce::Graphics &g,
                                            juce::Button &b,
                                            const juce::Colour &, bool,
                                            bool down) {
  auto r = b.getLocalBounds().toFloat().reduced(0.5f);
  g.setColour(retro::face());
  g.fillRect(r);
  g.setColour(juce::Colours::black);
  g.drawRect(r, 1.0f);
  auto inner = r.toType<int>().reduced(1);
  if (down)
    retro::bevelSunken(g, inner);
  else
    retro::bevelRaised(g, inner);
}

void RetroLookAndFeel::drawToggleButton(juce::Graphics &g,
                                        juce::ToggleButton &b, bool, bool) {
  auto r = b.getLocalBounds();
  juce::Rectangle<int> box(r.getX() + 2, r.getCentreY() - 7, 13, 13);
  g.setColour(juce::Colours::white);
  g.fillRect(box);
  g.setColour(juce::Colours::black);
  g.drawRect(box, 1);
  retro::bevelSunken(g, box.reduced(1));
  if (b.getToggleState()) {
    g.setColour(juce::Colours::black);
    auto tick = box.reduced(3, 2);
    g.drawLine((float)tick.getX(), (float)tick.getCentreY(),
               tick.getCentreX() - 1.0f, (float)tick.getBottom(), 2.5f);
    g.drawLine(tick.getCentreX() - 1.0f, (float)tick.getBottom(),
               (float)tick.getRight(), (float)tick.getY(), 2.5f);
  }
  g.setColour(b.findColour(juce::ToggleButton::textColourId));
  g.drawText(b.getButtonText(), r.withTrimmedLeft(box.getWidth() + 6),
             juce::Justification::centredLeft);
}

void RetroLookAndFeel::drawLinearSlider(
    juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
    float, float, const juce::Slider::SliderStyle style, juce::Slider &) {
  auto bounds = juce::Rectangle<int>(x, y, width, height);
  if (style == juce::Slider::LinearVertical) {
    int tx = bounds.getCentreX() - 4;
    g.setColour(juce::Colour(0xFF808080));
    g.fillRect(tx, bounds.getY(), 8, bounds.getHeight());
    retro::bevelSunken(g, juce::Rectangle<int>(tx, bounds.getY(), 8,
                                               bounds.getHeight()));
    juce::Rectangle<int> thumb(bounds.getX() + 2, int(sliderPos) - 5,
                               bounds.getWidth() - 4, 11);
    g.setColour(retro::face());
    g.fillRect(thumb);
    g.setColour(juce::Colours::black);
    g.drawRect(thumb, 1);
    retro::bevelRaised(g, thumb.reduced(1));
    g.setColour(juce::Colour(0xFF404040));
    g.drawHorizontalLine(thumb.getCentreY(), thumb.getX() + 3.0f,
                         thumb.getRight() - 3.0f);
  } else {
    int ty = bounds.getCentreY() - 4;
    g.setColour(juce::Colour(0xFF808080));
    g.fillRect(bounds.getX(), ty, bounds.getWidth(), 8);
    retro::bevelSunken(g,
                       juce::Rectangle<int>(bounds.getX(), ty,
                                            bounds.getWidth(), 8));
    juce::Rectangle<int> thumb(int(sliderPos) - 5, bounds.getY() + 2, 11,
                               bounds.getHeight() - 4);
    g.setColour(retro::face());
    g.fillRect(thumb);
    g.setColour(juce::Colours::black);
    g.drawRect(thumb, 1);
    retro::bevelRaised(g, thumb.reduced(1));
  }
}


} // namespace pg
