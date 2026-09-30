#!/bin/bash
# Lanzador PerenkeGain: ejecuta el binario si está compilado,
# si no intenta compilarlo, y como último recurso abre el proyecto.
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
BIN="$HERE/build/linux-release/PerenkeGain_artefacts/Release/PerenkeGain"

if [ -x "$BIN" ]; then
  exec "$BIN" "$@"
fi

if command -v cmake >/dev/null 2>&1; then
  cmake --preset linux-release && cmake --build --preset linux-release && exec "$BIN" "$@"
  echo "Fallo la compilación. Abro la carpeta del proyecto."
fi

xdg-open "$HERE" >/dev/null 2>&1 || nautilus "$HERE" >/dev/null 2>&1 &
