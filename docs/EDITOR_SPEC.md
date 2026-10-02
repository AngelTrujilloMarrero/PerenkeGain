# Editor Spec

## Layout (oscuro moderno)

```
+-----------------------------------------------------------+
| Deck A: título + tiempo | Transición + XFDER  | Deck B    |
| selector ▼ [▶][■]       | AUTOMIX + MASTER    | selector ▼ |
+-----------------------------------------------------------+
| [Añadir] [Editor de onda] [Normalizar lote] [Cerrar] | Ruta|
+-----------------------------------------------------------+
| Barra cian de progreso                                     |
+-----------------------------------------------------------+
| Buscador de YouTube: [consulta] [Buscar][Limpiar][Carpeta] |
|   hasta 3 resultados (info de descarga en la fila)          |
| Lista de reproducción: pistas con deck (A/B), arrastrar,    |
|   Subir/Bajar/Quitar, enviar a A/B                           |
+-----------------------------------------------------------+
| Bandeja inferior:                                           |
|   EQ 31 bandas (+ intensidad general / volumen)            |
|   Nivelador de sonoridad (LUFS)                            |
+-----------------------------------------------------------+
```

La **edición de onda (onda, corte/fade, escala, medidor master, Dividir y
Guardar como)** se abre en una **ventana independiente y redimensionable**
desde `Editor de onda...`.

## Reproductor, buscador y descarga

- Mesa con dos decks (título, tiempo posición/duración, selector y transporte),
  crossfader, **volumen master + medidor** y **Automix** (transición con fade
  corto/largo o corte) en el centro.
- `Añadir...` incorpora ficheros locales (selección múltiple).
- El **buscador de YouTube** (en la ventana principal) usa `yt-dlp` con
  `ytsearch`: lista resultados y `Descargar` extrae el elegido a MP3 con
  `ffmpeg`, lo normaliza y lo añade al reproductor.
- `Guardar como` vive en la ventana de onda (exporta los tramos marcados).

## EQ 31 bandas

- 31 sliders ISO 1/3 octava (`gain -12..+12 dB`) por banda, con presets por
  género (incluye géneros latinos) y `Personalizado` al editar a mano.
- `Intensidad general`: ganancia maestra de salida (0..100 %); al mínimo la
  salida queda en silencio.
- Procesado independiente por canal: 31x `juce::dsp::IIR peak Q=4.3`.

## Nivelador de sonoridad (LUFS)

- Objetivo de sonoridad percibida ajustable (def. -14 LUFS).
- Medición en tiempo real por bloques de 100 ms: LUFS momentáneo (400 ms) y
  de corto plazo (3 s), con puerta para no subir en silencio.
- Ballistics de ataque/liberación, límites de subida/bajada y limitador
  true-peak de seguridad. Solo afecta a la reproducción (no exporta).

## Splitter / Guardar

- Splitter: analiza el fichero al abrir el diálogo y propone cortes por
  silencio; se combinan con los marcadores manuales.
- Guardar: exporta los tramos como WAV 32-float o FLAC.
- Normalización por lotes (ventana aparte, tipo mp3gain): primero `Analizar`
  (mide sonoridad percibida y pico por fichero), luego `Aplicar pista` o
  `Aplicar álbum` al nivel objetivo en dB (escala mp3gain, 80–120). Conserva
  el formato de entrada; MP3 necesita lame/ffmpeg externos.
