#!/bin/bash
# Lanzador PerenkeGain: ejecuta el binario si está compilado,
# si no intenta compilarlo, y como último recurso lo explica sin
# cerrar el terminal de golpe.
HERE="$(cd "$(dirname "$0")" && pwd)"
BIN="$HERE/build/linux-release/PerenkeGain_artefacts/Release/PerenkeGain"

pause() {
  echo ""
  read -rp "Pulsa Enter para cerrar... " _ || true
}

if [ -x "$BIN" ]; then
  echo "Abriendo la ventana de PerenkeGain..."
  echo "(esta ventana se queda abierta mientras usas el programa;"
  echo " cierra PerenkeGain para terminar)"
  "$BIN" "$@"
  echo "PerenkeGain terminó con código $?."
  pause
  exit 0
fi

echo "=============================================="
echo " PerenkeGain aún no está compilado."
echo "----------------------------------------------"
echo " Proyecto: $HERE"
echo " Binario esperado:"
echo "  $BIN"
echo "=============================================="

if ! command -v cmake >/dev/null 2>&1; then
  echo ""
  echo "Falta 'cmake' (y probablemente 'ninja'). Instálalos con:"
  echo "  sudo apt-get install -y cmake ninja-build \\"
  echo "    libasound2-dev libjack-dev libfreetype6-dev \\"
  echo "    libx11-dev libxrandr-dev libxinerama-dev \\"
  echo "    libxcursor-dev mesa-common-dev"
  echo ""
  echo "Y luego compila con:"
  echo "  cd $HERE"
  echo "  cmake --preset linux-release"
  echo "  cmake --build --preset linux-release"
  echo ""
  echo "Abro la carpeta del proyecto..."
  xdg-open "$HERE" >/dev/null 2>&1 || nautilus "$HERE" >/dev/null 2>&1 &
  pause
  exit 1
fi

echo "Compilando (puede tardar varios minutos la primera vez)..."
if cmake --preset linux-release && cmake --build --preset linux-release; then
  echo "Compilación OK. Iniciando..."
  "$BIN" "$@"
  echo "PerenkeGain terminó con código $?."
  pause
  exit 0
fi

echo ""
echo "Fallo la compilación. Revisa los errores de arriba."
echo "Abro la carpeta del proyecto..."
xdg-open "$HERE" >/dev/null 2>&1 || nautilus "$HERE" >/dev/null 2>&1 &
pause
exit 1
