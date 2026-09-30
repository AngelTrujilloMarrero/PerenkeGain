#!/bin/bash
# Lanzador de desarrollo de PerenkeGain.
#
# La ventana normal se abre con el acceso directo del escritorio.
# Este script es solo para pulir el programa: compila (mostrando los
# errores de compilación) y ejecuta la app dejando en el terminal
# únicamente las líneas de error en tiempo de ejecución.
HERE="$(cd "$(dirname "$0")" && pwd)"
BIN="$HERE/build/linux-release/PerenkeGain_artefacts/Release/PerenkeGain"

export CPLUS_INCLUDE_PATH="/usr/include/freetype2:/usr/include/libpng16${CPLUS_INCLUDE_PATH:+:$CPLUS_INCLUDE_PATH}"

if ! command -v cmake >/dev/null 2>&1; then
  echo "Falta 'cmake' (y probablemente 'ninja'). Instálalos con:"
  echo "  sudo apt-get install -y cmake ninja-build libasound2-dev libjack-dev \\"
  echo "    libfreetype6-dev libx11-dev libxrandr-dev libxinerama-dev \\"
  echo "    libxcursor-dev mesa-common-dev libcurl4-openssl-dev libgtk-3-dev"
  exit 1
fi

echo "[PerenkeGain] Compilando..."
cmake --preset linux-release >/dev/null || exit 1
cmake --build --preset linux-release || {
  echo "[PerenkeGain] Fallo la compilación (errores arriba)."
  exit 1
}

echo "[PerenkeGain] Arrancando (solo se mostrarán errores)..."
"$BIN" "$@" 2>&1 | grep --line-buffered -iE \
  "error|fatal|assert|exception|failed|cannot|unable|segmentation|abort"
exit "${PIPESTATUS[0]}"
