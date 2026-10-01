# Editor Spec

## Layout (oscuro + bandeja retro)

```
+--------------------------------------------------+
| Ruta | Transporte (Play/Stop)                     |
+--------------------------------------------------+
| Info del fichero (formato, Hz, canales)          |
+--------------------------------------------------+
| Barra cian de progreso            Skip | ▲        |
+--------------------------------------------------+
| Fade In / Fade Out |        Waveform L/R     | L/R |
| Escala             |   marcadores + selección |master|
+--------------------------------------------------+
| Bandeja inferior:                                |
|   EQ 31 bandas (+ intensidad general / volumen)  |
|   Nivelador de sonoridad (LUFS)                  |
+--------------------------------------------------+
```

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
