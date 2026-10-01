#include "ui/BatchNormalizeDialog.h"
#include "storage/BatchNormalizer.h"
#include "types/Text.h"
#include "ui/BatchTask.h"
#include "ui/NormalizeReport.h"
#include "ui/RetroLookAndFeel.h"

namespace pg {

BatchNormalizeDialog::BatchNormalizeDialog() {
  for (auto *b : {&pickFile, &pickFolder, &pickOut, &analyzeB, &applyTrackB,
                  &applyAlbumB})
    addAndMakeVisible(*b);
  addAndMakeVisible(noClip);
  noClip.setToggleState(false, juce::dontSendNotification);
  noClip.onClick = [this] {
    if (!analyses.empty())
      renderReport();
  };
  for (auto *l : {&sourceL, &outL, &targetL}) {
    addAndMakeVisible(*l);
    l->setFont(juce::Font(juce::FontOptions(12.0f)));
  }
  targetL.setText(PG_T("Nivel objetivo:"), juce::dontSendNotification);
  sourceL.setText(PG_T("Sin selección"), juce::dontSendNotification);
  outL.setText(PG_T("Salida: (sin definir)"), juce::dontSendNotification);

  target.setRange(80.0, 120.0, 0.5);
  target.setValue(89.0, juce::dontSendNotification);
  target.setSliderStyle(juce::Slider::LinearHorizontal);
  target.setTextBoxStyle(juce::Slider::TextBoxRight, false, 90, 20);
  target.setTextValueSuffix(" dB");
  target.onValueChange = [this] {
    if (!analyses.empty())
      renderReport();
  };
  addAndMakeVisible(target);

  log.setMultiLine(true);
  log.setReadOnly(true);
  log.setFont(juce::Font(juce::FontOptions(12.0f)));
  addAndMakeVisible(log);

  auto onSource = [this](const juce::File &f) {
    if (f == juce::File{})
      return;
    source = f;
    if (outDir == juce::File{})
      outDir = f.isDirectory() ? f : f.getParentDirectory();
    analyses.clear();
    updateLabels();
  };
  pickFile.onClick = [this, onSource] {
    picker.choose(FilePicker::Mode::OpenFile, PG_T("Elegir archivo de audio"),
                  "*.wav;*.mp3;*.flac;*.ogg;*.aiff;*.aif", juce::File{},
                  onSource);
  };
  pickFolder.onClick = [this, onSource] {
    picker.choose(FilePicker::Mode::OpenDirectory,
                  PG_T("Elegir carpeta con audios"), {}, juce::File{}, onSource);
  };
  pickOut.onClick = [this] {
    picker.choose(FilePicker::Mode::OpenDirectory, PG_T("Carpeta de salida"),
                  {}, outDir, [this](const juce::File &d) {
                    if (d == juce::File{})
                      return;
                    outDir = d;
                    updateLabels();
                  });
  };
  analyzeB.onClick = [this] { analyze(); };
  applyTrackB.onClick = [this] { apply(false); };
  applyAlbumB.onClick = [this] { apply(true); };
  setBusy(false);
}

void BatchNormalizeDialog::setBusy(bool busy) {
  analyzeB.setEnabled(!busy);
  applyTrackB.setEnabled(!busy);
  applyAlbumB.setEnabled(!busy);
}

void BatchNormalizeDialog::updateLabels() {
  files = BatchNormalizer::collectFiles(source);
  sourceL.setText(source == juce::File{}
                      ? PG_T("Sin selección")
                      : PG_T("Origen: ") + source.getFullPathName() + " (" +
                            juce::String((int)files.size()) + " fichero(s))",
                  juce::dontSendNotification);
  outL.setText(outDir == juce::File{}
                   ? PG_T("Salida: (sin definir)")
                   : PG_T("Salida: ") + outDir.getFullPathName(),
               juce::dontSendNotification);
}

void BatchNormalizeDialog::analyze() {
  files = BatchNormalizer::collectFiles(source);
  if (files.empty()) {
    log.setText(PG_T("Selecciona un archivo o una carpeta con audios.\n"));
    return;
  }
  if (outDir == juce::File{})
    outDir = source.isDirectory() ? source : source.getParentDirectory();
  updateLabels();
  setBusy(true);
  log.setText(PG_T("Analizando...\n"));

  auto safe = juce::Component::SafePointer<BatchNormalizeDialog>(this);
  BatchTask::analyze(files, [safe](std::vector<NormalizeAnalysis> results) {
    if (safe == nullptr)
      return;
    safe->analyses = std::move(results);
    safe->setBusy(false);
    safe->renderReport(PG_T("Análisis:"));
  });
}

void BatchNormalizeDialog::renderReport(const juce::String &title) {
  const NormalizePlan plan =
      planNormalization(analyses, (float)target.getValue(),
                        noClip.getToggleState());
  log.setText(formatNormalizeReport(analyses, plan, title));
  log.moveCaretToEnd();
}

void BatchNormalizeDialog::apply(bool album) {
  if (analyses.empty()) {
    log.setText(PG_T("Pulsa primero 'Analizar'.\n"));
    return;
  }
  if (outDir == juce::File{})
    outDir = source.isDirectory() ? source : source.getParentDirectory();
  if (!outDir.exists())
    outDir.createDirectory();

  const NormalizePlan plan =
      planNormalization(analyses, (float)target.getValue(),
                        noClip.getToggleState());
  std::vector<juce::File> fs;
  std::vector<float> gs;
  for (size_t i = 0; i < analyses.size(); ++i) {
    if (!analyses[i].ok)
      continue;
    fs.push_back(analyses[i].file);
    gs.push_back(album ? plan.albumGainDb : plan.trackGains[i]);
  }
  if (fs.empty())
    return;

  setBusy(true);
  const juce::File dir = outDir;
  auto safe = juce::Component::SafePointer<BatchNormalizeDialog>(this);
  BatchTask::apply(
      fs, std::move(gs), dir,
      [safe, dir](juce::StringArray results) {
        if (safe == nullptr)
          return;
        safe->setBusy(false);
        safe->log.setText(results.joinIntoString("\n") + "\n\n" +
                          PG_T("Carpeta de salida: ") + dir.getFullPathName() +
                          "\n");
      });
}

void BatchNormalizeDialog::paint(juce::Graphics &g) {
  g.setColour(retro::face());
  g.fillAll();
  g.setColour(juce::Colours::black);
  g.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
  g.drawText(PG_T("Normalizar audios por lotes (mp3gain)"),
             getLocalBounds().removeFromTop(24), juce::Justification::centred);
}

void BatchNormalizeDialog::resized() {
  auto r = getLocalBounds().reduced(10);
  r.removeFromTop(18);
  auto row1 = r.removeFromTop(28);
  pickFile.setBounds(row1.removeFromLeft(110).reduced(2));
  pickFolder.setBounds(row1.removeFromLeft(110).reduced(2));
  pickOut.setBounds(row1.removeFromLeft(110).reduced(2));
  noClip.setBounds(row1.removeFromLeft(150).reduced(2));
  sourceL.setBounds(r.removeFromTop(20).reduced(2, 0));

  auto row2 = r.removeFromTop(28);
  targetL.setBounds(row2.removeFromLeft(130).reduced(2, 4));
  target.setBounds(row2.reduced(2));
  outL.setBounds(r.removeFromTop(20).reduced(2, 0));

  r.removeFromTop(4);
  auto row3 = r.removeFromTop(30);
  analyzeB.setBounds(row3.removeFromLeft(120).reduced(2));
  applyTrackB.setBounds(row3.removeFromLeft(130).reduced(2));
  applyAlbumB.setBounds(row3.removeFromLeft(130).reduced(2));

  r.removeFromTop(6);
  log.setBounds(r.reduced(2));
}

} // namespace pg
