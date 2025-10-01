#!/bin/bash
# Script de build automatizado

set -e  # Exit on error

BUILD_TYPE=${1:-Release}  # Debug o Release
BUILD_DIR="build"

echo "=========================================="
echo "CORDIC Softmax - Build Script"
echo "=========================================="
echo "Build type: $BUILD_TYPE"
echo ""

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
cmake --build . -j$(nproc)

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
