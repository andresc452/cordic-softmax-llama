# CORDIC Softmax for llama.cpp + FPGA

Implementación optimizada de softmax usando algoritmo CORDIC para integración con llama.cpp y posterior aceleración en FPGA.

## Estado del Proyecto

**Fase 1: ✅ COMPLETADA** - Implementación CPU funcionando  
**Fase 2: 🔄 EN PROGRESO** - Integración con llama.cpp  
**Fase 3: ⏳ PENDIENTE** - Síntesis HLS  
**Fase 4: ⏳ PENDIENTE** - Deployment FPGA

---

## Resultados de Fase 1

### Métricas de Precisión
- **Error CORDIC exp():** < 0.05%
- **Identidad hiperbólica:** cosh² - sinh² = 1.0 (exacto)
- **Tests pasados:** 5/5 (100%)

### Rendimiento (macOS M1, 8 cores)
| Vocabulario | Tiempo | MSE | Token Correcto |
|-------------|--------|-----|----------------|
| 100 tokens  | 66 μs  | 1.45e-11 | ✓ |
| 1K tokens   | 594 μs | 1.13e-07 | ✓ |
| 10K tokens  | 5.5 ms | 1.10e-06 | ✓ |

### Arquitectura Implementada

┌─────────────────┐
│ Preprocesador   │ → Mapeo exponencial: e^x = 2^n × e^(x')
└────────┬────────┘
         ↓
┌─────────────────┐
│ Iterador CORDIC │ → Rotaciones greedy (4-9 iteraciones)
└────────┬────────┘
         ↓ 
┌─────────────────┐
│ Postprocesador  │ → Cálculo: K = √(X²-Y²), e^x = cosh+sinh
└────────┬────────┘
         ↓
┌─────────────────┐
│ API Softmax     │ → Interfaz C/C++ para llama.cpp
└─────────────────┘

---

## Quick Start

### Requisitos
- CMake 3.15+
- GCC 7+ / Clang 10+ / AppleClang 11+
- C++17

### Build y Test
```bash
# Clonar
git clone https://github.com/andresc452/cordic-softmax-llama.git
cd cordic-softmax-llama

# Build automático
./build.sh Release

# Ejecutar todos los tests
cd build
ctest --output-on-failure

# Test individual del softmax
./test_softmax

