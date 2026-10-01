# PerenkeGain

![PerenkeGain](assets/logo-256.png)

Editor de audio moderno para digitalizar y restaurar vinilos y casetes.
Abre WAV/MP3/FLAC/OGG, muestra la forma de onda estéreo, divide en pistas
por silencio, aplica fade, un ecualizador gráfico de 31 bandas y un
nivelador de sonoridad en vivo, y guarda las pistas por separado.

## Funciones

- **Forma de onda estéreo** con zoom, marcadores y selección de tramos.
- **Divisor de pistas** por detección de silencio (marcadores arrastrables).
- **Ecualizador de 31 bandas** (ISO 1/3 octava) con presets por género
  (incluye géneros latinos) e intensidad general de salida.
- **Nivelador de sonoridad (LUFS)** en tiempo real con ataque/liberación,
  puerta y limitador true-peak; medidores momentáneo, corto plazo y pico.
- **Medidor master L/R** junto a la forma de onda.
- **Normalización por lotes tipo mp3gain**: analiza el volumen percibido de
  cada fichero, muestra la tabla y aplica la ganancia al objetivo en dB, por
  pista o por álbum. La salida conserva el formato de entrada (MP3 requiere
  `lame` o `ffmpeg` instalados).
- **Guardado multipista** WAV/FLAC por tramos.
- **Actualización automática** desde GitHub Releases.

## Decisiones

- Stack: **C++20 + JUCE 8 (Standalone App)**.
- Audio: **44.1–192 kHz, 32-bit float**.
- Licencia: **MIT**.
- Targets: **Linux + macOS** (CI). Windows pospuesto.
- UI: tema oscuro con bandeja inferior tipo mesa digital.

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
Source/app/      ventana, controlador y estado global
Source/audio/    engine, waveform, marcadores, transporte
Source/dsp/      silencio, splitter, de-click, de-hiss, EQ31, nivelador
Source/ui/       EQ, nivelador, normalización por lotes y diálogos
Source/storage/  carga, exportación y normalización de ficheros
Source/types/    tipos comunes de audio
```

## Flujo

`Abrir archivo -> Waveform L/R -> marcadores/tramos -> Fade -> EQ y
nivelador en vivo -> Splitter -> Guardar pistas WAV/FLAC`.

La normalización por lotes (`Normalizar lote...`) es independiente: elige un
fichero o carpeta, pulsa **Analizar** (mide volumen/pico en la escala mp3gain,
80–120 dB), ajusta el objetivo y pulsa **Aplicar pista** o **Aplicar álbum**.

Detalles de layout y DSP en `docs/EDITOR_SPEC.md`.
