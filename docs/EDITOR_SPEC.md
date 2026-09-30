# Editor Spec — reinterpretación moderna PolderbitS

## Referencias originales

- `pbrecorder_ss.jpg` (archive.org): ventana Recorder gris, VU verde-amarillo-rojo dBFS, combo calidad CD/DVD/radio/teléfono, botones pletina.
- Malavida 5799-1..4: Editor + Advanced desplegado + Save Tracks.
- Changelog v5: bordes redimensionables, cut-points grandes, split middle/start-end+fade, split equal/N, L/R separado, show clicks in red.

## Layout moderno (oscuro)

```
+--------------------------------------------------+
| Menu: Abrir | Guardar pistas | Splitter | A/B   |
+--------------------------------------------------+
| TrackMarkers (amarillo sobre negro, arrastrable) |
| Waveform L/R (verde claro, zoom H/V, click-der)  |
+--------------------------------------------------+
| Transporte: Play Stop | Trim | Fade-In Fade-Out  |
+--------------------------------------------------+
| [Advanced v] Click Crackle [x] Noise [x] EQ [x]   |
| EQ31: 31x gain vertical + 31x intensidad + master |
+--------------------------------------------------+
```

## EQ 31 con intensidad integrada (requisito usuario)

- Fila superior: `gain -12..+12 dB` por banda.
- Fila inferior: `intensidad 0..100%` por banda + `master 0..100%`.
- Fórmula: `effGain = gain * intensity * master`. Bypass respeta original.
- Presets: Flat, Vinilo (HP 20 Hz + -6 dB >16 k + +2 dB 2-4 k), Casete (low-pass suave + -hiss).
- DSP: 31x `juce::dsp::IIR peak Q=4.3`, `prepare()` por cambio de `sr`.

## Splitter / Save

- Splitter solo analiza al abrir diálogo (background thread).
- Save recuerda último formato (WAV 32-float / MP3 / FLAC), ID3 álbum+pista.
