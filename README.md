# CORDIC Softmax for llama.cpp + FPGA

Implementación optimizada de softmax usando algoritmo CORDIC para integración con llama.cpp y posterior aceleración en FPGA.

## Estado del Proyecto

### Fase 1: Implementación CPU (EN PROGRESO)
- [x] cordic_types.h - Tipos básicos y punto fijo
- [ ] cordic_preprocessor.h/.cpp - Preprocesamiento
- [ ] cordic_iterator.h/.cpp - Núcleo CORDIC
- [ ] cordic_postprocessor.h/.cpp - Postprocesamiento
- [ ] cordic_softmax.h/.cpp - API principal
- [ ] Integración con llama.cpp
- [ ] Validación y benchmarks

### Fase 2: Migración HLS (PENDIENTE)
### Fase 3: Aceleración FPGA (PENDIENTE)

## Quick Start
```bash
# Clonar
git clone https://github.com/tu-usuario/cordic-softmax-llama.git
cd cordic-softmax-llama

# Test actual
g++ -std=c++17 tests/test_types.cpp -o tests/test_types
./tests/test_types
