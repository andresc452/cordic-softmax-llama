/**
 * @file cordic_preprocessor.h
 * @brief Módulo de preprocesamiento para CORDIC Softmax
 * 
 * FUNCIÓN: Mapear la entrada al rango donde CORDIC converge eficientemente
 * 
 * ESTRATEGIA:
 * - Si |x| ≤ 0.347: usar directamente
 * - Si |x| > 0.347: aplicar mapeo exponencial e^x = 2^n × e^(x')
 */

#ifndef CORDIC_PREPROCESSOR_H
#define CORDIC_PREPROCESSOR_H

#include "cordic_types.h"

class CORDICPreprocessor {
public:
    /**
     * @brief Procesa la entrada y la mapea al rango de convergencia
     * @param input Valor de entrada (logit)
     * @param enable_debug Activar salida de debug
     * @return Resultado del preprocesamiento
     */
    static PreprocessResult processInput(float input, bool enable_debug = false);
    
    /**
     * @brief Inicializa las variables CORDIC para modo hiperbólico-rotación
     * @param preprocess_result Resultado del preprocesamiento
     * @return Estado inicial de CORDIC
     */
    static CORDICState initializeCORDICState(const PreprocessResult& preprocess_result);
    
    /**
     * @brief Imprime información de debug del preprocesamiento
     */
    static void printPreprocessInfo(const PreprocessResult& result);

private:
    /**
     * @brief Calcula el factor de reducción n
     * Fórmula: n = round(x / ln(2)) = round(x × INV_LN2)
     */
    static int calculateReductionFactor(float input);
    
    /**
     * @brief Ajusta n para garantizar |x'| ≤ CONVERGENCE_LIMIT
     */
    static float applyFineAdjustment(float mapped_input, int& reduction_factor);
    
    /**
     * @brief Valida que la entrada esté en rango aceptable
     */
    static bool validateInput(float input);
};

#endif // CORDIC_PREPROCESSOR_H
