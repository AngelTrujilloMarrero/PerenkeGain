#pragma once
#include <JuceHeader.h>

// Literales UTF-8 seguros: juce::String(const char*) es Latin-1 en JUCE 8.
#define PG_T(s) juce::String(juce::CharPointer_UTF8(s))
