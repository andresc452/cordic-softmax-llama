#!/bin/bash
# Script de build automatizado (compatible Linux/macOS)

set -e  # Exit on error

BUILD_TYPE=${1:-Release}
BUILD_DIR="build"

echo "=========================================="
echo "CORDIC Softmax - Build Script"
echo "=========================================="
echo "Build type: $BUILD_TYPE"
echo ""

# Detectar número de cores (compatible macOS/Linux)
if command -v nproc &> /dev/null; then
    NPROC=$(nproc)
elif command -v sysctl &> /dev/null; then
    NPROC=$(sysctl -n hw.ncpu)
else
    NPROC=4
fi

echo "Usando $NPROC cores"

# Limpiar build anterior si existe
if [ -d "$BUILD_DIR" ]; then
    echo "Limpiando build anterior..."
    rm -rf "$BUILD_DIR"
fi

# Crear directorio de build
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configurar con CMake
echo ""
echo "Configurando con CMake..."
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# Compilar
echo ""
echo "Compilando..."
cmake --build . -j$NPROC

# Ejecutar tests
echo ""
echo "Ejecutando tests..."
ctest --output-on-failure

echo ""
echo "=========================================="
echo "✅ BUILD EXITOSO"
echo "=========================================="
echo "Ejecutables en: $BUILD_DIR/"
echo ""
echo "Para ejecutar tests individuales:"
echo "  ./build/test_types"
echo "  ./build/test_preprocessor"
echo "  ./build/test_iterator"
