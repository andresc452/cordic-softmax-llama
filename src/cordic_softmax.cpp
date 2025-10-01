/**
 * @file cordic_softmax.cpp
 * @brief Implementación de la API unificada CORDIC
 */

#include "cordic_softmax.h"
#include <iostream>
#include <iomanip>
#include <cmath>

//==============================================================================
// IMPLEMENTACIÓN CORDICSoftmax
//==============================================================================

CORDICSoftmax::CORDICSoftmax(bool enable_debug) 
    : debug_mode(enable_debug) {
}

float CORDICSoftmax::calculateExp(float x) {
    // PASO 1: Preprocesamiento
    PreprocessResult prep = CORDICPreprocessor::processInput(x, debug_mode);
    
    // PASO 2: Inicializar estado CORDIC
    CORDICState initial = CORDICPreprocessor::initializeCORDICState(prep);
    
    // PASO 3: Iteraciones CORDIC
    IterationResult iter_result = iterator.performIterations(initial, debug_mode);
    
    // PASO 4: Postprocesamiento
    PostprocessResult post_result = CORDICPostprocessor::processResults(
        iter_result, prep, debug_mode);
    
    return post_result.exponential_value;
}

void CORDICSoftmax::computeSoftmax(const float* logits, float* probabilities, size_t size) {
    if (debug_mode) {
        std::cout << "\n=== SOFTMAX CORDIC ===" << std::endl;
        std::cout << "Procesando " << size << " elementos" << std::endl;
    }
    
    // PASO 1: Encontrar máximo para estabilización numérica
    float max_logit = *std::max_element(logits, logits + size);
    
    if (debug_mode) {
        std::cout << "Máximo logit: " << max_logit << std::endl;
    }
    
    // PASO 2: Calcular exponenciales estabilizadas
    float sum = 0.0f;
    for (size_t i = 0; i < size; i++) {
        float stabilized_logit = logits[i] - max_logit;
        probabilities[i] = calculateExp(stabilized_logit);
        sum += probabilities[i];
    }
    
    if (debug_mode) {
        std::cout << "Suma de exponenciales: " << sum << std::endl;
    }
    
    // PASO 3: Normalizar a probabilidades
    float inv_sum = 1.0f / sum;
    for (size_t i = 0; i < size; i++) {
        probabilities[i] *= inv_sum;
    }
    
    if (debug_mode) {
        // Verificar que suma = 1
        float verification_sum = 0.0f;
        for (size_t i = 0; i < size; i++) {
            verification_sum += probabilities[i];
        }
        std::cout << "Verificación suma: " << verification_sum << std::endl;
        std::cout << "Softmax completado" << std::endl;
    }
}

void CORDICSoftmax::calculateExpBatch(const float* inputs, float* outputs, size_t size) {
    for (size_t i = 0; i < size; i++) {
        outputs[i] = calculateExp(inputs[i]);
    }
}

void CORDICSoftmax::printConfiguration() {
    std::cout << "\n=== CONFIGURACIÓN CORDIC SOFTMAX ===" << std::endl;
    std::cout << "Precisión: " << CORDICConfig::WORD_WIDTH << "-bit punto fijo" << std::endl;
    std::cout << "  Bits fraccionales: " << CORDICConfig::FRAC_WIDTH << std::endl;
    std::cout << "  Resolución: " << (1.0 / (1 << CORDICConfig::FRAC_WIDTH)) << std::endl;
    std::cout << "Algoritmo: CORDIC hiperbólico con selección greedy" << std::endl;
    std::cout << "  Máximo iteraciones: " << CORDICConfig::MAX_ITERATIONS << std::endl;
    std::cout << "  Umbral convergencia: " << CORDICConfig::CONVERGENCE_THRESHOLD << std::endl;
    std::cout << "Rango softmax: [" << CORDICConfig::SOFTMAX_MIN_LOGIT 
              << ", " << CORDICConfig::SOFTMAX_MAX_LOGIT << "]" << std::endl;
    std::cout << "Error típico: < 0.1%" << std::endl;
}

//==============================================================================
// FUNCIONES C PARA LLAMA.CPP
//==============================================================================

// Instancia singleton para las funciones C
static CORDICSoftmax& getCORDICInstance() {
    static CORDICSoftmax instance(false);  // Sin debug para C API
    return instance;
}

extern "C" {

float llama_cordic_exp(float x) {
    return getCORDICInstance().calculateExp(x);
}

void llama_cordic_softmax(const float* logits, float* probs, size_t vocab_size) {
    getCORDICInstance().computeSoftmax(logits, probs, vocab_size);
}

}  // extern "C"
