# PerenkeGain

![PerenkeGain](assets/logo-256.png)

**Editor y reproductor** de audio moderno para digitalizar y restaurar
vinilos y casetes. Abre WAV/MP3/FLAC/OGG, reproduce con mesa DJ (dos decks,
crossfader y automix), muestra la forma de onda estéreo, divide en pistas por
silencio, aplica fade, ecualizador de 31 bandas y nivelador de sonoridad en
vivo, y descarga de YouTube Music con normalización.

## Funciones

- **Mesa DJ** con dos decks (A/B), cada uno con su selector, transporte,
  **tiempo (posición/duración)** y botones **Cargar**/**X** (abre un
  archivo o vacía el deck); crossfader, **volumen master con medidor** y
  **Automix** (transición automática con fade corto/largo o corte) en el
  centro. La lista incluye canciones locales y descargadas de YouTube.
- **Lista de reproducción central** con el deck asignado a cada pista;
  reordena arrastrando o con Subir/Bajar, quita y envía a A/B.
- **Buscador de YouTube Music** integrado en la ventana principal: busca,
  descarga el resultado elegido como MP3 normalizado y lo añade al
  reproductor (requiere `yt-dlp` + `ffmpeg`).
- **Editor de onda en ventana aparte**: forma de onda estéreo con zoom,
  marcadores, corte/fade, escala, dividir y **guardar pistas**.
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
- **Windows incluido**: la release trae `PerenkeGain-windows-x64.zip` con el
  `.exe` listo (descomprime y ejecuta; yt-dlp/ffmpeg con
  `winget install yt-dlp` / `winget install ffmpeg`).

## Descargas

Cada release publica tres paquetes desde
[GitHub Releases](https://github.com/AngelTrujilloMarrero/PerenkeGain/releases):

| Plataforma | Archivo | Cómo ejecutarlo |
| --- | --- | --- |
| Windows (x64) | `PerenkeGain-windows-x64.zip` | Descomprime y ejecuta `PerenkeGain.exe` |
| Linux (x86_64) | `PerenkeGain-linux-x86_64.tar.gz` | `tar -xzf … && ./PerenkeGain` |
| macOS | `PerenkeGain-macos.zip` | Abre `PerenkeGain.app` |

## Decisiones

- Stack: **C++20 + JUCE 8 (Standalone App)**.
- Audio: **44.1–192 kHz, 32-bit float**.
- Licencia: **MIT**.
- Targets: **Linux + macOS + Windows** (CI).
- UI: tema oscuro con bandeja inferior tipo mesa digital.

## Build (Linux/macOS/Windows)

```bash
# Requiere CMake 3.22+, Ninja, compilador C++20
cmake --preset linux-release   # o macos-release / windows-release
cmake --build --preset windows-release
```

En Windows usa el símbolo de desarrollo de Visual Studio (x64) para que
`cl` y `ninja` estén en el PATH; el artefacto queda en
`build/windows-release/PerenkeGain_artefacts/Release/PerenkeGain.exe`.

JUCE se trae por `FetchContent` (no hay submodule pesado en el repo).
En CI se compila en Ubuntu + macOS + Windows. Sin `cmake` local, revisa
`.github/workflows/build.yml`.

## Estructura (AGENTS.md: 1 fichero = 1 cosa, <200 líneas)

```
Source/app/      ventana principal, reproductor, modelos y controlador
Source/audio/    engine, waveform, transporte
Source/dsp/      silencio, splitter, de-click, de-hiss, EQ31, nivelador, sonoridad
Source/ui/       reproductor, editor de onda, EQ, nivelador y diálogos
Source/storage/  carga, exportación, normalización y descarga (yt-dlp)
Source/types/    tipos comunes de audio
```

## Flujo

`Añadir/Buscar en YouTube -> elegir en un deck (A/B) -> reproducir y mezclar
con el crossfader -> Editor de onda (marcadores, corte, fade) -> Dividir /
Guardar -> EQ y nivelador en vivo -> Exportar WAV/FLAC`.

La normalización por lotes (`Normalizar lote...`) es independiente: elige un
fichero o carpeta, pulsa **Analizar** (mide volumen/pico en la escala mp3gain,
80–120 dB), ajusta el objetivo y pulsa **Aplicar pista** o **Aplicar álbum**.

El buscador de YouTube usa `yt-dlp` y `ffmpeg`; sin ellos el panel indica cómo
instalarlos (`brew install yt-dlp ffmpeg`).

Detalles de layout y DSP en `docs/EDITOR_SPEC.md`.
