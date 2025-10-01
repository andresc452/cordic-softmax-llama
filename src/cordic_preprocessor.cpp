/**
 * @file cordic_preprocessor.cpp
 * @brief Implementación del preprocesador CORDIC
 */

#include "cordic_preprocessor.h"
#include <iostream>
#include <iomanip>
#include <cmath>

PreprocessResult CORDICPreprocessor::processInput(float input, bool enable_debug) {
    PreprocessResult result;
    result.original_input = input;
    
    if (enable_debug) {
        std::cout << "\n=== PREPROCESAMIENTO CORDIC ===" << std::endl;
        std::cout << "Entrada original: " << input << std::endl;
    }
    
    // PASO 1: Validar entrada
    if (!validateInput(input)) {
        if (enable_debug) {
            std::cout << "⚠ Entrada fuera de rango válido, saturando..." << std::endl;
        }
        if (input < CORDICConfig::SOFTMAX_MIN_LOGIT) {
            result.mapped_input = FixedPoint16(CORDICConfig::SOFTMAX_MIN_LOGIT);
        } else {
            result.mapped_input = FixedPoint16(CORDICConfig::SOFTMAX_MAX_LOGIT);
        }
        result.reduction_factor = 0;
        result.mapping_applied = true;
        return result;
    }
    
    // PASO 2: Decidir si necesita mapeo
    if (std::abs(input) <= CORDICConfig::CONVERGENCE_LIMIT) {
        result.mapped_input = FixedPoint16(input);
        result.reduction_factor = 0;
        result.mapping_applied = false;
        
        if (enable_debug) {
            std::cout << "✓ Entrada en rango de convergencia [-" 
                      << CORDICConfig::CONVERGENCE_LIMIT << ", +" 
                      << CORDICConfig::CONVERGENCE_LIMIT << "]" << std::endl;
            std::cout << "  No se requiere mapeo" << std::endl;
        }
    } else {
        if (enable_debug) {
            std::cout << "⚠ Entrada fuera del rango de convergencia" << std::endl;
            std::cout << "  Aplicando mapeo: e^x = 2^n × e^(x')" << std::endl;
        }
        
        int n = calculateReductionFactor(input);
        float x_mapped = input - n * CORDICConfig::LN2;
        
        if (enable_debug) {
            std::cout << "  Factor inicial n = " << n << std::endl;
            std::cout << "  x' inicial = " << input << " - " << n 
                      << " × ln(2) = " << x_mapped << std::endl;
        }
        
        x_mapped = applyFineAdjustment(x_mapped, n);
        
        result.mapped_input = FixedPoint16(x_mapped);
        result.reduction_factor = n;
        result.mapping_applied = true;
        
        if (enable_debug) {
            std::cout << "  Factor final n = " << n << std::endl;
            std::cout << "  x' final = " << x_mapped << std::endl;
            std::cout << "  Verificación: " << n << " × ln(2) + " << x_mapped 
                      << " = " << (n * CORDICConfig::LN2 + x_mapped) 
                      << " ≈ " << input << std::endl;
        }
    }
    
    return result;
}

CORDICState CORDICPreprocessor::initializeCORDICState(const PreprocessResult& preprocess_result) {
    CORDICState state;
    state.X = FixedPoint16(1.0f);
    state.Y = FixedPoint16(0.0f);
    state.Z = preprocess_result.mapped_input;
    state.iteration_count = 0;
    state.converged = false;
    
    return state;
}

void CORDICPreprocessor::printPreprocessInfo(const PreprocessResult& result) {
    std::cout << "\n--- INFORMACIÓN DE PREPROCESAMIENTO ---" << std::endl;
    std::cout << "Entrada original: " << result.original_input << std::endl;
    std::cout << "Entrada mapeada: " << result.mapped_input.toFloat() << std::endl;
    std::cout << "Factor de reducción n: " << result.reduction_factor << std::endl;
    std::cout << "Mapeo aplicado: " << (result.mapping_applied ? "Sí" : "No") << std::endl;
    
    if (result.mapping_applied) {
        float verification = result.reduction_factor * CORDICConfig::LN2 + 
                           result.mapped_input.toFloat();
        std::cout << "Verificación: " << result.reduction_factor << " × ln(2) + " 
                  << result.mapped_input.toFloat() << " = " << verification 
                  << " (original: " << result.original_input << ")" << std::endl;
        float error = std::abs(verification - result.original_input);
        std::cout << "Error de mapeo: " << std::scientific << std::setprecision(3) 
                  << error << std::endl;
    }
}

int CORDICPreprocessor::calculateReductionFactor(float input) {
    return static_cast<int>(std::round(input * CORDICConfig::INV_LN2));
}

float CORDICPreprocessor::applyFineAdjustment(float mapped_input, int& reduction_factor) {
    const float limit = CORDICConfig::CONVERGENCE_LIMIT;
    float adjusted_input = mapped_input;
    
    while (adjusted_input > limit) {
        reduction_factor++;
        adjusted_input -= CORDICConfig::LN2;
    }
    
    while (adjusted_input < -limit) {
        reduction_factor--;
        adjusted_input += CORDICConfig::LN2;
    }
    
    return adjusted_input;
}

bool CORDICPreprocessor::validateInput(float input) {
    if (std::isnan(input) || std::isinf(input)) {
        return false;
    }
    
    const float PRACTICAL_MIN = -15.0f;
    const float PRACTICAL_MAX = 15.0f;
    
    return input >= PRACTICAL_MIN && input <= PRACTICAL_MAX;
}
