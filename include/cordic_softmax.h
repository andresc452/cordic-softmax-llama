/**
 * @file cordic_softmax.h
 * @brief API unificada para CORDIC Softmax
 * 
 * Interfaz principal que integra los 4 módulos CORDIC:
 * 1. Preprocesador - Mapeo exponencial
 * 2. Iterador - Rotaciones CORDIC greedy
 * 3. Postprocesador - Cálculo de e^x
 * 4. API pública - Reemplazo de std::exp y softmax
 */

#ifndef CORDIC_SOFTMAX_H
#define CORDIC_SOFTMAX_H

#include "cordic_types.h"
#include "cordic_preprocessor.h"
#include "cordic_iterator.h"
#include "cordic_postprocessor.h"
#include <vector>
#include <algorithm>

/**
 * @class CORDICSoftmax
 * @brief Implementación completa de softmax usando CORDIC
 * 
 * Características:
 * - Reemplazo directo de std::exp con < 0.1% error
 * - Softmax estabilizada automática
 * - Compatible con vocabularios grandes (32K+ tokens)
 * - Hardware-friendly (preparado para HLS)
 */
class CORDICSoftmax {
private:
    CORDICIterator iterator;
    bool debug_mode;
    
public:
    /**
     * @brief Constructor
     * @param enable_debug Activar salida detallada
     */
    explicit CORDICSoftmax(bool enable_debug = false);
    
    /**
     * @brief Calcula e^x usando CORDIC (reemplazo de std::exp)
     * 
     * FLUJO:
     * 1. Preprocesar: mapear x al rango de convergencia
     * 2. Iterar: rotaciones CORDIC con selección greedy
     * 3. Postprocesar: extraer e^x y restaurar valor original
     * 
     * @param x Exponente de entrada
     * @return e^x con error < 0.1%
     */
    float calculateExp(float x);
    
    /**
     * @brief Softmax completo con estabilización automática
     * 
     * ALGORITMO:
     * softmax(x_i) = exp(x_i - max_x) / Σ(exp(x_j - max_x))
     * 
     * @param logits Array de entrada (logits del modelo)
     * @param probabilities Array de salida (probabilidades [0,1])
     * @param size Tamaño del vocabulario
     */
    void computeSoftmax(const float* logits, float* probabilities, size_t size);
    
    /**
     * @brief Versión vectorizada para múltiples exponenciales
     */
    void calculateExpBatch(const float* inputs, float* outputs, size_t size);
    
    /**
     * @brief Configuración
     */
    void setDebugMode(bool enable) { debug_mode = enable; }
    bool isDebugEnabled() const { return debug_mode; }
    
    /**
     * @brief Información de configuración
     */
    static void printConfiguration();
};

//==============================================================================
// FUNCIONES C PARA INTEGRACIÓN CON LLAMA.CPP
//==============================================================================

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reemplazo directo de expf() para llama.cpp
 * 
 * USO EN LLAMA.CPP:
 * ```c
 * // Reemplazar:
 * float prob = expf(logit);
 * 
 * // Por:
 * float prob = llama_cordic_exp(logit);
 * ```
 */
float llama_cordic_exp(float x);

/**
 * @brief Softmax completo optimizado para llama.cpp
 * 
 * USO EN LLAMA.CPP:
 * ```c
 * float logits[vocab_size];
 * float probs[vocab_size];
 * llama_cordic_softmax(logits, probs, vocab_size);
 * ```
 */
void llama_cordic_softmax(const float* logits, float* probs, size_t vocab_size);

#ifdef __cplusplus
}
#endif

//==============================================================================
// MACROS PARA FÁCIL INTEGRACIÓN
//==============================================================================

#ifdef USE_CORDIC_SOFTMAX
    #define EXP_FUNC(x) llama_cordic_exp(x)
    #define SOFTMAX_FUNC(logits, probs, size) llama_cordic_softmax(logits, probs, size)
#else
    #define EXP_FUNC(x) std::exp(x)
    #define SOFTMAX_FUNC(logits, probs, size) /* usar implementación estándar */
#endif

#endif // CORDIC_SOFTMAX_H
