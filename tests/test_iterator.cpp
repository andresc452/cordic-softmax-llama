#include "cordic_types.h"
#include "cordic_preprocessor.h"
#include "cordic_iterator.h"
#include <iostream>
#include <iomanip>
#include <cmath>

void testAngleTable() {
    std::cout << "\n========== TEST: TABLA DE ÁNGULOS ==========" << std::endl;
    
    AngleTable table;
    
    std::cout << "Tamaño de tabla: " << table.size() << " ángulos" << std::endl;
    
    // Verificar algunos ángulos conocidos
    std::cout << "\nVerificación de ángulos:" << std::endl;
    
    // α₁ = arctanh(0.5)
    const auto& entry1 = table.getEntry(1);
    double expected1 = std::atanh(0.5);
    std::cout << "α₁ = " << entry1.angle << " (esperado: " << expected1 << ")" << std::endl;
    std::cout << "  tanh(α₁) = " << entry1.tanh_value << " (esperado: 0.5)" << std::endl;
    std::cout << "  shift = " << entry1.shift_amount << std::endl;
    bool ok1 = std::abs(entry1.angle - expected1) < 1e-6 && 
               std::abs(entry1.tanh_value - 0.5) < 1e-6;
    std::cout << "  " << (ok1 ? "✓" : "✗") << std::endl;
    
    // α₂ = arctanh(0.25)
    const auto& entry2 = table.getEntry(2);
    double expected2 = std::atanh(0.25);
    std::cout << "α₂ = " << entry2.angle << " (esperado: " << expected2 << ")" << std::endl;
    bool ok2 = std::abs(entry2.angle - expected2) < 1e-6;
    std::cout << "  " << (ok2 ? "✓" : "✗") << std::endl;
    
    // Verificar orden decreciente
    std::cout << "\nVerificando orden decreciente..." << std::endl;
    bool order_ok = true;
    for (int i = 1; i < table.size(); i++) {
        if (table.getEntry(i).angle <= table.getEntry(i+1).angle) {
            order_ok = false;
            break;
        }
    }
    std::cout << "Orden correcto: " << (order_ok ? "✓" : "✗") << std::endl;
    
    // Mostrar tabla completa (opcional)
    // table.printTable();
}

void testBasicIteration(float input, const char* description) {
    std::cout << "\n--- Test: " << description << " ---" << std::endl;
    std::cout << "Input: " << input << std::endl;
    
    // Preprocesar
    PreprocessResult prep = CORDICPreprocessor::processInput(input, false);
    CORDICState initial = CORDICPreprocessor::initializeCORDICState(prep);
    
    std::cout << "Estado inicial:" << std::endl;
    std::cout << "  Z₀ = " << initial.Z.toFloat() << std::endl;
    
    // Iterar
    CORDICIterator iterator;
    IterationResult result = iterator.performIterations(initial, false);
    
    std::cout << "Resultado:" << std::endl;
    std::cout << "  Iteraciones: " << result.iterations_used << std::endl;
    std::cout << "  Convergió: " << (result.converged_successfully ? "Sí" : "No") << std::endl;
    std::cout << "  Z_final = " << result.final_state.Z.toFloat() << std::endl;
    std::cout << "  X_final = " << result.final_state.X.toFloat() << std::endl;
    std::cout << "  Y_final = " << result.final_state.Y.toFloat() << std::endl;
    
    // Verificar convergencia
    float z_final = std::abs(result.final_state.Z.toFloat());
    bool converged = z_final < CORDICConfig::CONVERGENCE_THRESHOLD * 10;  // Tolerancia amplia
    std::cout << "  |Z_final| = " << z_final << " " << (converged ? "✓" : "✗") << std::endl;
    
    // Verificar que iteraciones ≤ MAX_ITERATIONS
    bool iter_ok = result.iterations_used <= CORDICConfig::MAX_ITERATIONS;
    std::cout << "  Iteraciones ≤ " << CORDICConfig::MAX_ITERATIONS << ": " 
              << (iter_ok ? "✓" : "✗") << std::endl;
    
    // Ángulos seleccionados
    std::cout << "  Ángulos: [";
    for (size_t i = 0; i < result.selected_angles.size(); i++) {
        std::cout << result.selected_angles[i];
        if (i < result.selected_angles.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

void testIterationDetail() {
    std::cout << "\n========== TEST DETALLADO: ITERACIÓN ==========" << std::endl;
    
    float input = 0.2f;
    std::cout << "Input: " << input << " (valor pequeño, convergencia rápida)" << std::endl;
    
    PreprocessResult prep = CORDICPreprocessor::processInput(input, false);
    CORDICState initial = CORDICPreprocessor::initializeCORDICState(prep);
    
    CORDICIterator iterator;
    IterationResult result = iterator.performIterations(initial, true);  // Debug ON
    
    std::cout << "\n✓ Iteración detallada completada" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "TEST: cordic_iterator" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        // Test 1: Tabla de ángulos
        testAngleTable();
        
        // Test 2: Iteraciones básicas
        std::cout << "\n========== TESTS: ITERACIONES BÁSICAS ==========" << std::endl;
        testBasicIteration(0.0f, "x = 0 (trivial)");
        testBasicIteration(0.1f, "x = 0.1 (pequeño)");
        testBasicIteration(0.3f, "x = 0.3 (dentro del límite)");
        testBasicIteration(1.0f, "x = 1.0 (con mapeo)");
        testBasicIteration(2.5f, "x = 2.5 (moderado)");
        testBasicIteration(-1.5f, "x = -1.5 (negativo)");
        
        // Test 3: Iteración detallada
        testIterationDetail();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "✅ TODOS LOS TESTS COMPLETADOS" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ ERROR: " << e.what() << std::endl;
        return 1;
    }
}
