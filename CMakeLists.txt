cmake_minimum_required(VERSION 3.22)
project(PerenkeGain VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.10
)
FetchContent_MakeAvailable(JUCE)

juce_add_gui_app(PerenkeGain
  PRODUCT_NAME "PerenkeGain"
  COMPANY_NAME "PerenkeGain"
  BUNDLE_ID "com.perenkegain.editor"
  ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/assets/logo-512.png"
  ICON_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/assets/logo-48.png"
  NEEDS_WEB_BROWSER TRUE
  NEEDS_CURL TRUE
)

juce_generate_juce_header(PerenkeGain)

target_sources(PerenkeGain PRIVATE
  Source/Main.cpp
  Source/app/MainWindow.cpp
  Source/app/ApplicationState.cpp
  Source/audio/AudioEngine.cpp
  Source/audio/WaveformComponent.cpp
  Source/audio/TrackMarkersComponent.cpp
  Source/audio/TransportComponent.cpp
  Source/dsp/SilenceDetector.cpp
  Source/dsp/TrackSplitterService.cpp
  Source/dsp/RumbleFilter.cpp
  Source/dsp/DeClicker.cpp
  Source/dsp/DeHiss.cpp
  Source/dsp/Eq31BandProcessor.cpp
  Source/dsp/DspChain.cpp
  Source/ui/AdvancedPanel.cpp
  Source/ui/EqualizerComponent.cpp
  Source/ui/TrackSplitterDialog.cpp
  Source/ui/SaveTracksDialog.cpp
  Source/storage/ProjectStore.cpp
  Source/storage/ExportManager.cpp
)

target_include_directories(PerenkeGain PRIVATE Source)

target_link_libraries(PerenkeGain PRIVATE
  juce::juce_audio_basics
  juce::juce_audio_devices
  juce::juce_audio_formats
  juce::juce_audio_processors
  juce::juce_audio_utils
  juce::juce_dsp
  juce::juce_gui_basics
  juce::juce_gui_extra
)

target_compile_definitions(PerenkeGain PRIVATE
  JUCE_USE_MP3AUDIOFORMAT=1
  JUCE_USE_OGGVORBISAUDIOFORMAT=1
  JUCE_USE_FLACAUDIOFORMAT=1
  JUCE_STANDALONE_APPLICATION=1
)
