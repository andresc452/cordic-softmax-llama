/**
 * @file cordic_postprocessor.h
 * @brief Módulo de postprocesamiento para CORDIC Softmax
 * 
 * FUNCIÓN: Convertir resultados CORDIC en exponencial final
 * 
 * PROCESO:
 * 1. Calcular factor de escala K basado en ángulos usados
 * 2. Extraer cosh(x') y sinh(x') de X_final, Y_final
 * 3. Calcular e^(x') = cosh(x') + sinh(x')
 * 4. Restaurar valor original: e^x = e^(x') × 2^n
 */

#ifndef CORDIC_POSTPROCESSOR_H
#define CORDIC_POSTPROCESSOR_H

#include "cordic_types.h"
#include <cmath>

class CORDICPostprocessor {
public:
    /**
     * @brief Procesa resultados CORDIC para obtener exponencial final
     * 
     * @param iteration_result Resultado de iteraciones CORDIC
     * @param preprocess_result Resultado del preprocesamiento
     * @param enable_debug Mostrar información detallada
     * @return Resultado completo del postprocesamiento
     */
    static PostprocessResult processResults(
        const IterationResult& iteration_result,
        const PreprocessResult& preprocess_result,
        bool enable_debug = false
    );
    
    /**
     * @brief Muestra información detallada del postprocesamiento
     */
    static void printPostprocessInfo(const PostprocessResult& result);

private:
    /**
     * @brief Calcula factor de escala K
     * 
     * FÓRMULA: K = ∏(i) cosh(α_i) para ángulos utilizados
     * 
     * @param selected_angles Índices de ángulos usados
     * @return Factor de escala
     */
    static float calculateScalingFactor(const std::vector<int>& selected_angles);
    
    /**
     * @brief Extrae funciones hiperbólicas
     * 
     * cosh(x') = X_final / K
     * sinh(x') = Y_final / K
     * 
     * @param final_state Estado final CORDIC
     * @param scaling_factor Factor K
     * @param cosh_result [out] Valor de cosh
     * @param sinh_result [out] Valor de sinh
     */
    static void extractHyperbolicFunctions(
        const CORDICState& final_state,
        float scaling_factor,
        float& cosh_result,
        float& sinh_result
    );
    
    /**
     * @brief Calcula exponencial usando identidad
     * 
     * IDENTIDAD: e^x = cosh(x) + sinh(x)
     * 
     * @param cosh_val Valor de cosh
     * @param sinh_val Valor de sinh
     * @return Exponencial
     */
    static float calculateExponential(float cosh_val, float sinh_val);
    
    /**
     * @brief Restaura valor original aplicando mapeo inverso
     * 
     * e^x = e^(x') × 2^n
     * 
     * @param exp_mapped Exponencial del valor mapeado
     * @param preprocess_result Información de mapeo
     * @return Exponencial del valor original
     */
    static float restoreOriginalValue(
        float exp_mapped,
        const PreprocessResult& preprocess_result
    );
    
    /**
     * @brief Calcula error relativo vs std::exp
     * 
     * @param computed_value Valor calculado por CORDIC
     * @param original_input Entrada original
     * @return Error relativo
     */
    static float calculateError(float computed_value, float original_input);
};

#endif // CORDIC_POSTPROCESSOR_H
