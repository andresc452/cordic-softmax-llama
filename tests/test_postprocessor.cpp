#include "cordic_types.h"
#include "cordic_preprocessor.h"
#include "cordic_iterator.h"
#include "cordic_postprocessor.h"
#include <iostream>
#include <iomanip>
#include <cmath>

void testCompleteFlow(float input, const char* description) {
    std::cout << "\n========== Test: " << description << " ==========" << std::endl;
    std::cout << "Input: " << input << std::endl;
    
    // PASO 1: Preprocesar
    PreprocessResult prep = CORDICPreprocessor::processInput(input, false);
    
    // PASO 2: Inicializar y iterar
    CORDICState initial = CORDICPreprocessor::initializeCORDICState(prep);
    CORDICIterator iterator;
    IterationResult iter_result = iterator.performIterations(initial, false);
    
    // PASO 3: Postprocesar
    PostprocessResult post_result = CORDICPostprocessor::processResults(
        iter_result, prep, false);
    
    // Verificación
    float reference = std::exp(input);
    float cordic_value = post_result.exponential_value;
    float error_percent = post_result.relative_error * 100.0f;
    
    std::cout << "\nResultados:" << std::endl;
    std::cout << "  std::exp(" << input << ") = " << reference << std::endl;
    std::cout << "  CORDIC exp(" << input << ") = " << cordic_value << std::endl;
    std::cout << "  Error: " << std::fixed << std::setprecision(4) << error_percent << "%" << std::endl;
    std::cout << "  Iteraciones: " << iter_result.iterations_used << std::endl;
    std::cout << "  Factor K: " << post_result.scaling_factor << std::endl;
    
    // Verificar identidad hiperbólica
    float identity = post_result.cosh_value * post_result.cosh_value - 
                    post_result.sinh_value * post_result.sinh_value;
    std::cout << "  cosh² - sinh²: " << identity << " (debe ≈ 1.0)" << std::endl;
    
    // Verificación de éxito
    bool error_ok = error_percent < 1.0f;  // Error < 1%
    bool identity_ok = std::abs(identity - 1.0f) < 0.1f;
    
    std::cout << "\n  Error < 1%: " << (error_ok ? "✓" : "✗") << std::endl;
    std::cout << "  Identidad OK: " << (identity_ok ? "✓" : "✗") << std::endl;
    
    if (error_ok && identity_ok) {
        std::cout << "  ✅ TEST PASADO" << std::endl;
    } else {
        std::cout << "  ❌ TEST FALLIDO" << std::endl;
    }
}

void testDetailedFlow() {
    std::cout << "\n========== TEST DETALLADO: FLUJO COMPLETO ==========" << std::endl;
    
    float input = 2.5f;
    std::cout << "Input: " << input << " (flujo con debug)" << std::endl;
    
    // Ejecutar con debug habilitado
    PreprocessResult prep = CORDICPreprocessor::processInput(input, true);
    CORDICState initial = CORDICPreprocessor::initializeCORDICState(prep);
    CORDICIterator iterator;
    IterationResult iter_result = iterator.performIterations(initial, true);
    
    // FIX: Usar el resultado para evitar warning
    PostprocessResult post_result = CORDICPostprocessor::processResults(
        iter_result, prep, true);
    
    // Mostrar resultado final
    std::cout << "\nResultado final: " << post_result.exponential_value << std::endl;
    std::cout << "✓ Flujo detallado completado" << std::endl;
}

void testErrorAnalysis() {
    std::cout << "\n========== ANÁLISIS DE ERROR ==========" << std::endl;
    
    struct TestCase {
        float input;
        float max_error_percent;
    };
    
    TestCase cases[] = {
        {0.0f, 0.1f},
        {0.5f, 0.5f},
        {1.0f, 0.5f},
        {2.0f, 1.0f},
        {3.0f, 1.0f},
        {5.5f, 1.0f},
        {-1.0f, 0.5f},
        {-3.0f, 1.0f},
        {8.0f, 1.0f},
        {-8.0f, 1.0f}
    };
    
    int passed = 0;
    int total = sizeof(cases) / sizeof(cases[0]);
    
    std::cout << "\nInput\t| Referencia\t| CORDIC\t| Error %\t| Estado" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& test : cases) {
        PreprocessResult prep = CORDICPreprocessor::processInput(test.input, false);
        CORDICState initial = CORDICPreprocessor::initializeCORDICState(prep);
        CORDICIterator iterator;
        IterationResult iter_result = iterator.performIterations(initial, false);
        PostprocessResult post_result = CORDICPostprocessor::processResults(
            iter_result, prep, false);
        
        float reference = std::exp(test.input);
        float error_percent = post_result.relative_error * 100.0f;
        bool pass = error_percent <= test.max_error_percent;
        
        if (pass) passed++;
        
        std::cout << std::fixed << std::setprecision(2) << test.input << "\t| "
                  << std::setprecision(4) << reference << "\t| "
                  << post_result.exponential_value << "\t| "
                  << std::setprecision(3) << error_percent << "%\t\t| "
                  << (pass ? "✓" : "✗") << std::endl;
    }
    
    std::cout << std::string(70, '-') << std::endl;
    std::cout << "Resultado: " << passed << "/" << total << " tests pasados" << std::endl;
    
    if (passed == total) {
        std::cout << "✅ TODOS LOS TESTS DE ERROR PASARON" << std::endl;
    } else {
        std::cout << "⚠ " << (total - passed) << " tests fallaron" << std::endl;
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "TEST: cordic_postprocessor" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        // Tests de flujo completo
        testCompleteFlow(0.0f, "x = 0 (trivial)");
        testCompleteFlow(1.0f, "x = 1 (e)");
        testCompleteFlow(2.5f, "x = 2.5 (moderado)");
        testCompleteFlow(5.5f, "x = 5.5 (grande)");
        testCompleteFlow(-2.0f, "x = -2.0 (negativo)");
        
        // Test detallado con debug
        testDetailedFlow();
        
        // Análisis de error extensivo
        testErrorAnalysis();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "✅ TODOS LOS TESTS COMPLETADOS" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ ERROR: " << e.what() << std::endl;
        return 1;
    }
}
