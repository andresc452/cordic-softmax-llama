/**
 * @file cordic_postprocessor.cpp
 * @brief Implementación del postprocesador CORDIC
 */

#include "cordic_postprocessor.h"
#include <iostream>
#include <iomanip>
#include <cmath>

PostprocessResult CORDICPostprocessor::processResults(
    const IterationResult& iteration_result,
    const PreprocessResult& preprocess_result,
    bool enable_debug
) {
    PostprocessResult result;
    
    if (enable_debug) {
        std::cout << "\n=== POSTPROCESAMIENTO CORDIC ===" << std::endl;
        std::cout << "Procesando resultado de " << iteration_result.iterations_used 
                  << " iteraciones" << std::endl;
    }
    
    // Paso 1: Calcular factor de escala K
    result.scaling_factor = calculateScalingFactor(iteration_result.selected_angles);
    
    if (enable_debug) {
        std::cout << "\nPaso 1: Factor de escala K = " << result.scaling_factor << std::endl;
        std::cout << "Ángulos utilizados: [";
        for (size_t i = 0; i < iteration_result.selected_angles.size(); i++) {
            std::cout << iteration_result.selected_angles[i];
            if (i < iteration_result.selected_angles.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
    // Paso 2: Extraer funciones hiperbólicas
    extractHyperbolicFunctions(
        iteration_result.final_state,
        result.scaling_factor,
        result.cosh_value,
        result.sinh_value
    );
    
    if (enable_debug) {
        std::cout << "\nPaso 2: Funciones hiperbólicas extraídas" << std::endl;
        std::cout << "cosh(x') = " << std::fixed << std::setprecision(8) 
                  << result.cosh_value << std::endl;
        std::cout << "sinh(x') = " << result.sinh_value << std::endl;
        
        // Verificar identidad hiperbólica: cosh² - sinh² = 1
        float identity_check = result.cosh_value * result.cosh_value - 
                              result.sinh_value * result.sinh_value;
        std::cout << "Verificación cosh² - sinh² = " << identity_check 
                  << " (debe ≈ 1.0)" << std::endl;
    }
    
    // Paso 3: Calcular exponencial del valor mapeado
    float exp_mapped = calculateExponential(result.cosh_value, result.sinh_value);
    
    if (enable_debug) {
        std::cout << "\nPaso 3: Exponencial calculada" << std::endl;
        std::cout << "e^(x') = cosh + sinh = " << exp_mapped << std::endl;
    }
    
    // Paso 4: Restaurar valor original
    result.exponential_value = restoreOriginalValue(exp_mapped, preprocess_result);
    
    if (enable_debug) {
        std::cout << "\nPaso 4: Valor restaurado" << std::endl;
        if (preprocess_result.mapping_applied) {
            std::cout << "e^x = 2^" << preprocess_result.reduction_factor 
                      << " × " << exp_mapped << " = " << result.exponential_value << std::endl;
        } else {
            std::cout << "e^x = " << result.exponential_value << " (sin mapeo)" << std::endl;
        }
    }
    
    // Paso 5: Calcular error para validación
    result.relative_error = calculateError(result.exponential_value, 
                                          preprocess_result.original_input);
    
    if (enable_debug) {
        float reference = std::exp(preprocess_result.original_input);
        std::cout << "\nPaso 5: Validación" << std::endl;
        std::cout << "Valor referencia std::exp: " << reference << std::endl;
        std::cout << "Valor CORDIC: " << result.exponential_value << std::endl;
        std::cout << "Error relativo: " << std::fixed << std::setprecision(6) 
                  << (result.relative_error * 100) << "%" << std::endl;
    }
    
    return result;
}

float CORDICPostprocessor::calculateScalingFactor(const std::vector<int>& selected_angles) {
    if (selected_angles.empty()) {
        return 1.0f;
    }
    
    // MÉTODO CORRECTO: Calcular K multiplicando cosh(α_i) 
    // para CADA ángulo en la lista (incluyendo repeticiones)
    float K = 1.0f;
    
    for (int angle_idx : selected_angles) {
        float tanh_val = std::pow(2.0f, -angle_idx);
        float angle = std::atanh(tanh_val);
        float cosh_angle = std::cosh(angle);
        
        K *= cosh_angle;
    }
    
    return K;
}

void CORDICPostprocessor::extractHyperbolicFunctions(
    const CORDICState& final_state,
    float scaling_factor,
    float& cosh_result,
    float& sinh_result
) {
    float x_final = final_state.X.toFloat();
    float y_final = final_state.Y.toFloat();
    
    cosh_result = x_final / scaling_factor;
    sinh_result = y_final / scaling_factor;
}

float CORDICPostprocessor::calculateExponential(float cosh_val, float sinh_val) {
    return cosh_val + sinh_val;
}

float CORDICPostprocessor::restoreOriginalValue(
    float exp_mapped,
    const PreprocessResult& preprocess_result
) {
    if (!preprocess_result.mapping_applied) {
        return exp_mapped;
    }
    
    float power_of_2 = std::pow(2.0f, preprocess_result.reduction_factor);
    return exp_mapped * power_of_2;
}

float CORDICPostprocessor::calculateError(float computed_value, float original_input) {
    float reference_value = std::exp(original_input);
    
    if (std::abs(reference_value) < 1e-10f) {
        return std::abs(computed_value);
    }
    
    return std::abs(computed_value - reference_value) / std::abs(reference_value);
}

void CORDICPostprocessor::printPostprocessInfo(const PostprocessResult& result) {
    std::cout << "\n--- INFORMACIÓN DE POSTPROCESAMIENTO ---" << std::endl;
    std::cout << "Exponencial final: " << std::fixed << std::setprecision(8) 
              << result.exponential_value << std::endl;
    std::cout << "Factor de escala K: " << result.scaling_factor << std::endl;
    std::cout << "cosh(x'): " << result.cosh_value << std::endl;
    std::cout << "sinh(x'): " << result.sinh_value << std::endl;
    std::cout << "Error relativo: " << std::setprecision(6) 
              << (result.relative_error * 100) << "%" << std::endl;
    
    if (result.relative_error < 0.001f) {
        std::cout << "Precisión: EXCELENTE (<0.1%)" << std::endl;
    } else if (result.relative_error < 0.01f) {
        std::cout << "Precisión: MUY BUENA (<1%)" << std::endl;
    } else if (result.relative_error < 0.05f) {
        std::cout << "Precisión: BUENA (<5%)" << std::endl;
    } else {
        std::cout << "Precisión: ACEPTABLE (>5%)" << std::endl;
    }
}
