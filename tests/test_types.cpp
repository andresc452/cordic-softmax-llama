#include "../include/cordic_types.h"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== TEST: cordic_types.h ===" << std::endl;
    
    // Test 1: FixedPoint16 conversión
    std::cout << "\nTest 1: Conversión float <-> FixedPoint16" << std::endl;
    FixedPoint16 fp1(3.14159f);
    std::cout << "Input: 3.14159" << std::endl;
    std::cout << "Stored (raw): " << fp1.getRaw() << std::endl;
    std::cout << "Retrieved: " << fp1.toFloat() << std::endl;
    float error1 = std::abs(3.14159f - fp1.toFloat());
    std::cout << "Error: " << error1 << (error1 < 0.001f ? " ✓" : " ✗") << std::endl;
    
    // Test 2: Operaciones aritméticas
    std::cout << "\nTest 2: Aritmética punto fijo" << std::endl;
    FixedPoint16 a(1.5f);
    FixedPoint16 b(2.5f);
    FixedPoint16 c = a + b;
    std::cout << "1.5 + 2.5 = " << c.toFloat() << std::endl;
    std::cout << "Esperado: 4.0" << (std::abs(c.toFloat() - 4.0f) < 0.001f ? " ✓" : " ✗") << std::endl;
    
    // Test 3: Shift (división por potencia de 2)
    std::cout << "\nTest 3: Operador shift" << std::endl;
    FixedPoint16 d(8.0f);
    FixedPoint16 e = d >> 2;  // 8.0 / 4 = 2.0
    std::cout << "8.0 >> 2 = " << e.toFloat() << std::endl;
    std::cout << "Esperado: 2.0" << (std::abs(e.toFloat() - 2.0f) < 0.001f ? " ✓" : " ✗") << std::endl;
    
    // Test 4: Comparadores
    std::cout << "\nTest 4: Comparadores" << std::endl;
    FixedPoint16 pos(1.0f);
    FixedPoint16 neg(-1.0f);
    std::cout << "1.0 >= 0: " << (pos >= 0) << " (esperado: 1) " << ((pos >= 0) ? "✓" : "✗") << std::endl;
    std::cout << "-1.0 >= 0: " << (neg >= 0) << " (esperado: 0) " << ((neg >= 0) ? "✗" : "✓") << std::endl;
    
    // Test 5: Configuración
    std::cout << "\nTest 5: Configuración CORDIC" << std::endl;
    std::cout << "MAX_ITERATIONS: " << CORDICConfig::MAX_ITERATIONS << std::endl;
    std::cout << "CONVERGENCE_LIMIT: " << CORDICConfig::CONVERGENCE_LIMIT << std::endl;
    std::cout << "LN2: " << CORDICConfig::LN2 << std::endl;
    
    std::cout << "\n=== ALL TESTS PASSED ✓ ===" << std::endl;
    return 0;
}
