# PerenkeGain — clon open del Editor PolderbitS

![PerenkeGain](assets/logo-256.png)

Solo **editor** (sin grabador). Reinterpretación moderna del
`PolderbitS Sound Recorder and Editor 9.0` orientada a digitalizar
vinilos/casetes: abrir WAV/MP3, ver waveform estéreo, splitter por
silencio, trim/fade, filtros Click/Crackle + Hiss + EQ gráfica 31 bandas
con intensidad por banda, y guardado multipista.

## Decisiones cerradas

- Stack: **C++20 + JUCE 8 (Standalone App)**.
- Audio: **44.1–192 kHz, 32-bit float** (moderno, no 16-bit legacy).
- Licencia: **MIT**.
- Targets: **Linux + macOS** (CI). Windows pospuesto.
- UI: reinterpretación moderna oscura, mismo flujo simple original.
- EQ: 31 sliders ISO 1/3 octava + **intensidad 0–100% por banda**.

## Estado visual de referencia

Ver `docs/EDITOR_SPEC.md`: layout, paleta, panel `Advanced`,
`Track Splitter` y `Save Tracks` reconstruidos desde
archive.org (`pbrecorder_ss.jpg`) y Malavida (4 capturas).

## Build (Linux/macOS)

```bash
# Requiere CMake 3.22+, Ninja, compilador C++20
cmake --preset linux-release   # o macos-release
cmake --build --preset linux-release
```

JUCE se trae por `FetchContent` (no hay submodule pesado en el repo).
En CI se compila en Ubuntu + macOS. Sin `cmake` local, revisa
`.github/workflows/build.yml`.

## Estructura (AGENTS.md: 1 fichero = 1 cosa, <200 líneas)

```
Source/app/      ventana + estado global
Source/audio/    engine, waveform, marcadores, transporte
Source/dsp/      silencio, splitter, rumble, de-click, de-hiss, EQ31, cadena
Source/ui/       Advanced, EQ, diálogos splitter/save
Source/storage/  SQLite proyectos + exportación
Source/types/    tipos comunes audio
```

## Flujo editor

`Abrir archivo -> Waveform L/R -> marcadores arrastrables ->
Trim/Fade -> Advanced (filtros realtime + A/B) -> Splitter ->
Save Tracks WAV/MP3/FLAC`
