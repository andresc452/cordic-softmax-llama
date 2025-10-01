#include "../include/cordic_types.h"
#include "../include/cordic_preprocessor.h"
#include <iostream>
#include <iomanip>
#include <cmath>

void testCase(float input, bool should_map, const char* description) {
    std::cout << "\n--- Test: " << description << " ---" << std::endl;
    std::cout << "Input: " << input << std::endl;
    
    PreprocessResult result = CORDICPreprocessor::processInput(input, false);
    
    std::cout << "Mapped input: " << result.mapped_input.toFloat() << std::endl;
    std::cout << "Reduction factor: " << result.reduction_factor << std::endl;
    std::cout << "Mapping applied: " << (result.mapping_applied ? "Yes" : "No") << std::endl;
    
    // Verificar resultado
    float x_prime = result.mapped_input.toFloat();
    bool in_range = std::abs(x_prime) <= CORDICConfig::CONVERGENCE_LIMIT + 0.001;  // Tolerancia
    
    std::cout << "|x'| = " << std::abs(x_prime) << " ≤ " 
              << CORDICConfig::CONVERGENCE_LIMIT << ": " 
              << (in_range ? "✓" : "✗") << std::endl;
    
    // Verificar mapeo
    if (result.mapping_applied) {
        float reconstructed = result.reduction_factor * CORDICConfig::LN2 + x_prime;
        float error = std::abs(reconstructed - input);
        std::cout << "Reconstruction: " << reconstructed 
                  << " (error: " << error << ")" << std::endl;
        
        if (error > 0.01) {
            std::cout << "✗ ERROR: Mapeo incorrecto" << std::endl;
        } else {
            std::cout << "✓ Mapeo correcto" << std::endl;
        }
    }
    
    // Verificar expectativa
    if (result.mapping_applied != should_map) {
        std::cout << "✗ ERROR: Se esperaba mapping=" << should_map << std::endl;
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "TEST: cordic_preprocessor" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\nConfiguración:" << std::endl;
    std::cout << "CONVERGENCE_LIMIT = " << CORDICConfig::CONVERGENCE_LIMIT << std::endl;
    std::cout << "LN2 = " << CORDICConfig::LN2 << std::endl;
    std::cout << "INV_LN2 = " << CORDICConfig::INV_LN2 << std::endl;
    
    // TESTS: Casos sin mapeo
    std::cout << "\n========== CASOS SIN MAPEO ==========" << std::endl;
    testCase(0.0f, false, "x = 0 (caso trivial)");
    testCase(0.1f, false, "x = 0.1 (pequeño positivo)");
    testCase(-0.2f, false, "x = -0.2 (pequeño negativo)");
    testCase(0.34f, false, "x = 0.34 (justo dentro del límite)");
    
    // TESTS: Casos con mapeo
    std::cout << "\n========== CASOS CON MAPEO ==========" << std::endl;
    testCase(1.0f, true, "x = 1.0 (moderado)");
    testCase(2.5f, true, "x = 2.5 (moderado)");
    testCase(5.5f, true, "x = 5.5 (grande)");
    testCase(-3.2f, true, "x = -3.2 (negativo)");
    testCase(8.0f, true, "x = 8.0 (límite superior softmax)");
    testCase(-8.0f, true, "x = -8.0 (límite inferior softmax)");
    
    // TESTS: Casos especiales
    std::cout << "\n========== CASOS ESPECIALES ==========" << std::endl;
    testCase(0.347f, false, "x = 0.347 (exactamente en límite)");
    testCase(0.348f, true, "x = 0.348 (justo fuera límite)");
    
    // TEST: Inicialización de estado CORDIC
    std::cout << "\n========== TEST INICIALIZACIÓN ==========" << std::endl;
    PreprocessResult result = CORDICPreprocessor::processInput(2.5f, false);
    CORDICState state = CORDICPreprocessor::initializeCORDICState(result);
    
    std::cout << "Estado inicial CORDIC:" << std::endl;
    std::cout << "  X₀ = " << state.X.toFloat() << " (esperado: 1.0)" << std::endl;
    std::cout << "  Y₀ = " << state.Y.toFloat() << " (esperado: 0.0)" << std::endl;
    std::cout << "  Z₀ = " << state.Z.toFloat() << std::endl;
    
    bool init_ok = (std::abs(state.X.toFloat() - 1.0f) < 0.001f) &&
                   (std::abs(state.Y.toFloat() - 0.0f) < 0.001f);
    std::cout << (init_ok ? "✓" : "✗") << " Inicialización correcta" << std::endl;
    
    // TEST: Validación de entrada
    std::cout << "\n========== TEST VALIDACIÓN ==========" << std::endl;
    
    float nan_val = 0.0f / 0.0f;
    PreprocessResult nan_result = CORDICPreprocessor::processInput(nan_val, false);
    std::cout << "NaN saturado a: " << nan_result.mapped_input.toFloat() 
              << (nan_result.mapping_applied ? " ✓" : " ✗") << std::endl;
    
    PreprocessResult extreme_result = CORDICPreprocessor::processInput(20.0f, false);
    std::cout << "20.0 saturado a: " << extreme_result.mapped_input.toFloat() 
              << (extreme_result.mapping_applied ? " ✓" : " ✗") << std::endl;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "TESTS COMPLETADOS" << std::endl;
    std::cout << "Revisar visualmente los resultados ✓/✗" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
