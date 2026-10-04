#include "ui/LoadReadout.h"
#include "audio/AudioEngine.h"
#include "types/Text.h"

#if JUCE_WINDOWS
#include <windows.h>
#else
#include <sys/resource.h>
#endif

namespace pg {
namespace {

// Tiempo de CPU (usuario+systema) consumido por el proceso, en segundos.
double processCpuSeconds() {
#if JUCE_WINDOWS
  FILETIME creation, exit, kernel, user;
  if (!GetProcessTimes(GetCurrentProcess(), &creation, &exit, &kernel, &user))
    return -1.0;
  ULARGE_INTEGER k{}, u{};
  k.LowPart = kernel.dwLowDateTime;
  k.HighPart = kernel.dwHighDateTime;
  u.LowPart = user.dwLowDateTime;
  u.HighPart = user.dwHighDateTime;
  return (double)(k.QuadPart + u.QuadPart) * 1.0e-7;
#else
  struct rusage ru{};
  if (getrusage(RUSAGE_SELF, &ru) != 0)
    return -1.0;
  return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec * 1.0e-6 +
         (double)ru.ru_stime.tv_sec + (double)ru.ru_stime.tv_usec * 1.0e-6;
#endif
}

} // namespace

LoadReadout::LoadReadout(AudioEngine &e) : engine(e) {
  setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
  setColour(juce::Label::textColourId, juce::Colour(0xFF7CFF3C));
  setJustificationType(juce::Justification::centred);
  setInterceptsMouseClicks(false, false);
  setText(PG_T("CPU --%  AUD --%"), juce::dontSendNotification);
  startTimerHz(2);
}

void LoadReadout::timerCallback() {
  const double cpu = processCpuSeconds();
  const double wall = juce::Time::getMillisecondCounterHiRes() * 0.001;
  int cpuPct = -1;
  if (lastCpuSeconds >= 0.0 && wall > lastWallSeconds) {
    cpuPct = juce::roundToInt((cpu - lastCpuSeconds) /
                              (wall - lastWallSeconds) * 100.0);
    cpuPct = juce::jmax(0, cpuPct);
  }
  lastCpuSeconds = cpu;
  lastWallSeconds = wall;

  const int audPct = juce::roundToInt(engine.audioLoad() * 100.0);
  setText(juce::String("CPU ") +
              (cpuPct < 0 ? juce::String("--") : juce::String(cpuPct)) +
              "%   AUD " + juce::String(audPct) + "%",
          juce::dontSendNotification);
}

} // namespace pg
