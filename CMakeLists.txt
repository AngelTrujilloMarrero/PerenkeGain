cmake_minimum_required(VERSION 3.22)
project(PerenkeGain VERSION 0.8.3 LANGUAGES C CXX)

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

# Imagenes embebidas (logo de la barra y portada de "Acerca de").
juce_add_binary_data(PerenkeGainData SOURCES
  assets/logobadge.png
  assets/logohero.jpeg
)

target_sources(PerenkeGain PRIVATE
  Source/Main.cpp
  Source/app/MainWindow.cpp
  Source/app/EditorRootComponent.cpp
  Source/app/EditorController.cpp
  Source/app/PlaybackTicker.cpp
  Source/app/DialogLauncher.cpp
  Source/app/PlaylistModel.cpp
  Source/app/MarkerModel.cpp
  Source/app/WaveformWindow.cpp
  Source/ui/ModernLookAndFeel.cpp
  Source/ui/CyanProgressBar.cpp
  Source/ui/BottomDockComponent.cpp
  Source/ui/DeckComponent.cpp
  Source/ui/DeckButtons.cpp
  Source/ui/DeckDragDrop.cpp
  Source/ui/TransportButton.cpp
  Source/ui/MixerBarComponent.cpp
  Source/ui/PlaylistComponent.cpp
  Source/ui/WaveformEditorComponent.cpp
  Source/ui/YouTubeSearchPanel.cpp
  Source/ui/YouTubePanelTools.cpp
  Source/ui/YouTubeSearchTask.cpp
  Source/ui/YouTubeTask.cpp
  Source/ui/MixerStripComponent.cpp
  Source/ui/MixerRowComponent.cpp
  Source/dsp/BandLevelAnalyzer.cpp
  Source/app/ApplicationState.cpp
  Source/storage/EqSettingsStore.cpp
  Source/audio/AudioEngine.cpp
  Source/audio/WaveformComponent.cpp
  Source/audio/TransportComponent.cpp
  Source/dsp/SilenceDetector.cpp
  Source/dsp/TrackSplitterService.cpp
  Source/dsp/RumbleFilter.cpp
  Source/dsp/DeClicker.cpp
  Source/dsp/DeHiss.cpp
  Source/dsp/Eq31BandProcessor.cpp
  Source/dsp/LoudnessMeterRT.cpp
  Source/dsp/LoudnessAnalyzer.cpp
  Source/dsp/Leveler.cpp
  Source/dsp/DspChain.cpp
  Source/ui/BatchNormalizeDialog.cpp
  Source/ui/BatchTask.cpp
  Source/ui/FilePicker.cpp
  Source/ui/NormalizeReport.cpp
  Source/ui/EqualizerComponent.cpp
  Source/ui/EqPresetBar.cpp
  Source/ui/TrackSplitterDialog.cpp
  Source/ui/SaveTracksDialog.cpp
  Source/ui/LevelerPanel.cpp
  Source/ui/MasterLevelBar.cpp
  Source/storage/ProjectStore.cpp
  Source/storage/ExportManager.cpp
  Source/storage/AudioFileLoader.cpp
  Source/storage/AudioEncoder.cpp
  Source/storage/BatchNormalizer.cpp
  Source/storage/ExternalTool.cpp
  Source/storage/YouTubeDownloader.cpp
  Source/storage/YouTubeSearch.cpp
  Source/storage/ToolInstaller.cpp
  Source/updater/Version.cpp
  Source/updater/UpdateInfo.cpp
  Source/updater/GitHubReleaseClient.cpp
  Source/updater/UpdateChecker.cpp
  Source/updater/UpdateDownloader.cpp
  Source/updater/UpdateInstaller.cpp
  Source/ui/UpdateBannerComponent.cpp
  Source/ui/UpdateDialog.cpp
  Source/ui/AboutDialog.cpp
)

target_include_directories(PerenkeGain PRIVATE Source)

target_link_libraries(PerenkeGain PRIVATE
  PerenkeGainData
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
  PG_VERSION="${PROJECT_VERSION}"
)
