#include "storage/AudioEncoder.h"
#include "types/Text.h"

namespace pg {

namespace {

juce::File findExecutable(const juce::String &name) {
  auto path = juce::SystemStats::getEnvironmentVariable("PATH", {});
  for (auto &dir : juce::StringArray::fromTokens(path, ":", "")) {
    if (dir.isEmpty())
      continue;
    juce::File f = juce::File(dir).getChildFile(name);
    if (f.existsAsFile())
      return f;
  }
  return {};
}

bool writeFormat(const juce::File &out, const juce::AudioBuffer<float> &buf,
                 double sampleRate, const juce::String &ext, juce::String &error) {
  juce::WavAudioFormat wav;
  juce::AiffAudioFormat aiff;
  juce::FlacAudioFormat flac;
  juce::OggVorbisAudioFormat ogg;
  juce::AudioFormat *fmt = nullptr;
  int bits = 24;
  if (ext == "wav" || ext == "wave")
    fmt = &wav;
  else if (ext == "aif" || ext == "aiff")
    fmt = &aiff;
  else if (ext == "flac")
    fmt = &flac;
  else if (ext == "ogg" || ext == "oga") {
    fmt = &ogg;
    bits = 16;
  }
  if (fmt == nullptr) {
    error = "Formato no soportado: " + ext;
    return false;
  }

  std::unique_ptr<juce::OutputStream> os = out.createOutputStream();
  if (os == nullptr) {
    error = "No se pudo crear " + out.getFullPathName();
    return false;
  }
  auto options = juce::AudioFormatWriterOptions{}
                     .withSampleRate(sampleRate)
                     .withNumChannels(buf.getNumChannels())
                     .withBitsPerSample(bits)
                     .withQualityOptionIndex(0)
                     .withSampleFormat(
                         juce::AudioFormatWriterOptions::SampleFormat::integral);
  auto w = fmt->createWriterFor(os, options);
  if (w == nullptr) {
    error = "No se pudo escribir " + ext;
    return false;
  }
  w->writeFromAudioSampleBuffer(buf, 0, buf.getNumSamples());
  return true;
}

// MP3: pasa por un WAV temporal y llama a lame o ffmpeg.
bool writeMp3(const juce::File &out, const juce::AudioBuffer<float> &buf,
              double sampleRate, juce::String &error) {
  auto tmp = juce::File::getSpecialLocation(juce::File::tempDirectory)
                 .getNonexistentChildFile("pgnorm", ".wav");
  if (!writeFormat(tmp, buf, sampleRate, "wav", error))
    return false;

  auto lame = findExecutable("lame");
  auto ffmpeg = findExecutable("ffmpeg");
  juce::ChildProcess proc;
  bool started = false;
  if (lame != juce::File{}) {
    started = proc.start(juce::StringArray{
        lame.getFullPathName(), "--quiet", "-b", "320", "-h",
        tmp.getFullPathName(), out.getFullPathName()});
  } else if (ffmpeg != juce::File{}) {
    started = proc.start(juce::StringArray{
        ffmpeg.getFullPathName(), "-y", "-loglevel", "error", "-i",
        tmp.getFullPathName(), "-b:a", "320k", out.getFullPathName()});
  } else {
    error = PG_T("Sin codificador MP3 (instala lame o ffmpeg)");
    tmp.deleteFile();
    return false;
  }

  bool ok = false;
  if (started) {
    proc.waitForProcessToFinish(120000);
    ok = proc.getExitCode() == 0 && out.existsAsFile();
    if (!ok)
      error = PG_T("El codificador MP3 falló");
  } else {
    error = PG_T("No se pudo lanzar el codificador MP3");
  }
  tmp.deleteFile();
  return ok;
}

} // namespace

bool AudioEncoder::write(const juce::File &out, const juce::AudioBuffer<float> &buf,
                         double sampleRate, const juce::String &extension,
                         juce::String &error) {
  const juce::String ext =
      extension.toLowerCase().trimCharactersAtStart(".");
  if (ext == "mp3")
    return writeMp3(out, buf, sampleRate, error);
  return writeFormat(out, buf, sampleRate, ext, error);
}

} // namespace pg
