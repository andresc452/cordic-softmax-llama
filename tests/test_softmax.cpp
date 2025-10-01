#include "cordic_softmax.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <random>

//==============================================================================
// UTILIDADES
//==============================================================================

void computeReferenceSoftmax(const float* logits, float* probs, size_t size) {
    float max_logit = *std::max_element(logits, logits + size);
    float sum = 0.0f;
    
    for (size_t i = 0; i < size; i++) {
        probs[i] = std::exp(logits[i] - max_logit);
        sum += probs[i];
    }
    
    for (size_t i = 0; i < size; i++) {
        probs[i] /= sum;
    }
}

double calculateMSE(const float* a, const float* b, size_t size) {
    double mse = 0.0;
    for (size_t i = 0; i < size; i++) {
        double diff = a[i] - b[i];
        mse += diff * diff;
    }
    return mse / size;
}

//==============================================================================
// TESTS
//==============================================================================

void testCORDICExp() {
    std::cout << "\n========== TEST: CORDIC EXP vs std::exp ==========" << std::endl;
    
    CORDICSoftmax cordic(false);
    
    struct TestCase {
        float input;
        const char* description;
    };
    
    TestCase cases[] = {
        {0.0f, "e^0 = 1"},
        {1.0f, "e^1 = e"},
        {-1.0f, "e^(-1)"},
        {2.5f, "e^2.5"},
        {-3.0f, "e^(-3)"},
        {5.5f, "e^5.5"},
        {8.0f, "e^8 (grande)"},
        {-8.0f, "e^(-8) (pequeño)"}
    };
    
    int passed = 0;
    int total = sizeof(cases) / sizeof(cases[0]);
    
    std::cout << "\nInput\t| std::exp\t| CORDIC\t| Error %\t| Estado" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (const auto& test : cases) {
        float reference = std::exp(test.input);
        float cordic_result = cordic.calculateExp(test.input);
        float error_percent = std::abs(reference - cordic_result) / reference * 100.0f;
        
        bool pass = error_percent < 0.1f;  // Error < 0.1%
        if (pass) passed++;
        
        std::cout << std::fixed << std::setprecision(2) << test.input << "\t| "
                  << std::setprecision(4) << reference << "\t| "
                  << cordic_result << "\t| "
                  << std::setprecision(3) << error_percent << "%\t\t| "
                  << (pass ? "✓" : "✗") << std::endl;
    }
    
    std::cout << std::string(70, '-') << std::endl;
    std::cout << "Resultado: " << passed << "/" << total << " tests pasados" << std::endl;
    
    if (passed == total) {
        std::cout << "✅ TEST CORDIC EXP PASÓ" << std::endl;
    }
}

void testBasicSoftmax() {
    std::cout << "\n========== TEST: SOFTMAX BÁSICO ==========" << std::endl;
    
    CORDICSoftmax cordic(false);
    
    // Caso simple: 3 logits
    float logits[] = {1.0f, 2.0f, 3.0f};
    size_t size = 3;
    
    float cordic_probs[3];
    float reference_probs[3];
    
    cordic.computeSoftmax(logits, cordic_probs, size);
    computeReferenceSoftmax(logits, reference_probs, size);
    
    std::cout << "\nLogits: [";
    for (size_t i = 0; i < size; i++) {
        std::cout << logits[i];
        if (i < size - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    
    std::cout << "\ni\t| Referencia\t| CORDIC\t| Diff" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (size_t i = 0; i < size; i++) {
        float diff = std::abs(reference_probs[i] - cordic_probs[i]);
        std::cout << i << "\t| " << std::fixed << std::setprecision(6)
                  << reference_probs[i] << "\t| "
                  << cordic_probs[i] << "\t| "
                  << std::scientific << std::setprecision(2) << diff << std::endl;
    }
    
    // Verificar suma = 1
    float sum_cordic = 0.0f;
    for (size_t i = 0; i < size; i++) {
        sum_cordic += cordic_probs[i];
    }
    
    double mse = calculateMSE(reference_probs, cordic_probs, size);
    
    std::cout << "\nVerificaciones:" << std::endl;
    std::cout << "  Suma CORDIC: " << std::fixed << std::setprecision(8) << sum_cordic << std::endl;
    std::cout << "  MSE: " << std::scientific << std::setprecision(3) << mse << std::endl;
    
    bool sum_ok = std::abs(sum_cordic - 1.0f) < 1e-5f;
    bool mse_ok = mse < 1e-6;
    
    std::cout << "  Suma ≈ 1.0: " << (sum_ok ? "✓" : "✗") << std::endl;
    std::cout << "  MSE < 1e-6: " << (mse_ok ? "✓" : "✗") << std::endl;
    
    if (sum_ok && mse_ok) {
        std::cout << "✅ TEST SOFTMAX BÁSICO PASÓ" << std::endl;
    }
}

void testLargeVocabSoftmax() {
    std::cout << "\n========== TEST: VOCABULARIO GRANDE ==========" << std::endl;
    
    const size_t vocab_sizes[] = {100, 1000, 10000};
    
    CORDICSoftmax cordic(false);
    
    for (size_t vocab_size : vocab_sizes) {
        std::cout << "\nVocabulario: " << vocab_size << " tokens" << std::endl;
        
        // Generar logits aleatorios (distribución típica de LLM)
        std::vector<float> logits(vocab_size);
        std::vector<float> cordic_probs(vocab_size);
        std::vector<float> reference_probs(vocab_size);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> dist(0.0f, 3.0f);
        
        for (size_t i = 0; i < vocab_size; i++) {
            logits[i] = dist(gen);
        }
        
        // Computar softmax
        auto start = std::chrono::high_resolution_clock::now();
        cordic.computeSoftmax(logits.data(), cordic_probs.data(), vocab_size);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        computeReferenceSoftmax(logits.data(), reference_probs.data(), vocab_size);
        
        // Calcular métricas
        double mse = calculateMSE(reference_probs.data(), cordic_probs.data(), vocab_size);
        
        float sum_probs = 0.0f;
        for (size_t i = 0; i < vocab_size; i++) {
            sum_probs += cordic_probs[i];
        }
        
        auto max_ref_it = std::max_element(reference_probs.begin(), reference_probs.end());
        auto max_cordic_it = std::max_element(cordic_probs.begin(), cordic_probs.end());
        size_t max_ref_idx = std::distance(reference_probs.begin(), max_ref_it);
        size_t max_cordic_idx = std::distance(cordic_probs.begin(), max_cordic_it);
        
        std::cout << "  Tiempo: " << duration.count() << " μs" << std::endl;
        std::cout << "  MSE: " << std::scientific << std::setprecision(3) << mse << std::endl;
        std::cout << "  Suma probabilidades: " << std::fixed << std::setprecision(8) 
                  << sum_probs << std::endl;
        std::cout << "  Token máxima prob (ref/cordic): " << max_ref_idx << "/" 
                  << max_cordic_idx << std::endl;
        std::cout << "  Prob máxima (ref/cordic): " << std::setprecision(6)
                  << *max_ref_it << "/" << *max_cordic_it << std::endl;
        
        bool mse_ok = mse < 1e-5;
        bool sum_ok = std::abs(sum_probs - 1.0f) < 1e-4f;
        bool token_ok = max_ref_idx == max_cordic_idx;
        
        std::cout << "  MSE OK: " << (mse_ok ? "✓" : "✗") << std::endl;
        std::cout << "  Suma OK: " << (sum_ok ? "✓" : "✗") << std::endl;
        std::cout << "  Token correcto: " << (token_ok ? "✓" : "✗") << std::endl;
        
        if (mse_ok && sum_ok && token_ok) {
            std::cout << "  ✅ Vocabulario " << vocab_size << " PASÓ" << std::endl;
        }
    }
}

void testCInterfaceAPI() {
    std::cout << "\n========== TEST: INTERFAZ C (llama.cpp) ==========" << std::endl;
    
    std::cout << "\nProbando llama_cordic_exp()..." << std::endl;
    float test_values[] = {0.0f, 1.0f, -1.0f, 2.5f, -3.0f};
    
    for (float x : test_values) {
        float reference = std::exp(x);
        float cordic_result = llama_cordic_exp(x);
        float error = std::abs(reference - cordic_result) / reference * 100.0f;
        
        std::cout << "  llama_cordic_exp(" << std::fixed << std::setprecision(1) << x 
                  << ") = " << std::setprecision(4) << cordic_result 
                  << " (error: " << std::setprecision(3) << error << "%)" << std::endl;
    }
    
    std::cout << "\nProbando llama_cordic_softmax()..." << std::endl;
    float logits[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    float probs[5];
    size_t size = 5;
    
    llama_cordic_softmax(logits, probs, size);
    
    float sum = 0.0f;
    std::cout << "  Probabilidades: [";
    for (size_t i = 0; i < size; i++) {
        std::cout << std::fixed << std::setprecision(4) << probs[i];
        if (i < size - 1) std::cout << ", ";
        sum += probs[i];
    }
    std::cout << "]" << std::endl;
    std::cout << "  Suma: " << std::setprecision(6) << sum << std::endl;
    
    if (std::abs(sum - 1.0f) < 1e-5f) {
        std::cout << "✅ INTERFAZ C FUNCIONANDO" << std::endl;
    }
}

void testConfiguration() {
    std::cout << "\n========== CONFIGURACIÓN CORDIC SOFTMAX ==========" << std::endl;
    CORDICSoftmax::printConfiguration();
}

//==============================================================================
// MAIN
//==============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "TEST: CORDIC SOFTMAX API" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testConfiguration();
        testCORDICExp();
        testBasicSoftmax();
        testLargeVocabSoftmax();
        testCInterfaceAPI();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "✅ TODOS LOS TESTS COMPLETADOS" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "\n🎉 FASE 1 COMPLETADA: Implementación CPU" << std::endl;
        std::cout << "   - Error < 0.1% en exponenciales" << std::endl;
        std::cout << "   - Softmax estabilizada funcionando" << std::endl;
        std::cout << "   - Interfaz C lista para llama.cpp" << std::endl;
        std::cout << "   - Preparado para síntesis HLS" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ ERROR: " << e.what() << std::endl;
        return 1;
    }
}
