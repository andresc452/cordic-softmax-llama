/**
 * @file cordic_types.h
 * @brief Tipos de datos fundamentales para CORDIC Softmax
 * 
 * Configuración: Q3.12 (16-bit fixed-point)
 * - 1 bit signo
 * - 3 bits enteros → rango [-8, +7.999]
 * - 12 bits fraccionales → resolución 0.000244
 */

#ifndef CORDIC_TYPES_H
#define CORDIC_TYPES_H

#include <cstdint>
#include <cmath>
#include <vector>

//==============================================================================
// CONFIGURACIÓN GLOBAL
//==============================================================================

namespace CORDICConfig {
    // Configuración punto fijo
    constexpr int WORD_WIDTH = 16;
    constexpr int FRAC_WIDTH = 12;
    constexpr int INT_WIDTH = 3;
    
    // Parámetros del algoritmo
    constexpr int MAX_ITERATIONS = 6;
    constexpr double CONVERGENCE_THRESHOLD = 1e-4;
    
    // Constantes matemáticas
    constexpr double LN2 = 0.693147180559945;
    constexpr double INV_LN2 = 1.442695040888963;
    constexpr double CONVERGENCE_LIMIT = LN2 / 2.0;  // 0.347
    
    // Rangos para softmax
    constexpr float SOFTMAX_MIN_LOGIT = -8.0f;
    constexpr float SOFTMAX_MAX_LOGIT = 8.0f;
}

//==============================================================================
// CLASE PUNTO FIJO
//==============================================================================

class FixedPoint16 {
private:
    int16_t value;
    static constexpr int16_t FIXED_ONE = 1 << CORDICConfig::FRAC_WIDTH;
    static constexpr int16_t MAX_VAL = INT16_MAX;
    static constexpr int16_t MIN_VAL = INT16_MIN;
    
public:
    FixedPoint16() : value(0) {}
    
    explicit FixedPoint16(float val) {
        int32_t temp = static_cast<int32_t>(val * FIXED_ONE);
        if (temp > MAX_VAL) temp = MAX_VAL;
        if (temp < MIN_VAL) temp = MIN_VAL;
        value = static_cast<int16_t>(temp);
    }
    
    explicit FixedPoint16(double val) {
        int32_t temp = static_cast<int32_t>(val * FIXED_ONE);
        if (temp > MAX_VAL) temp = MAX_VAL;
        if (temp < MIN_VAL) temp = MIN_VAL;
        value = static_cast<int16_t>(temp);
    }
    
    float toFloat() const {
        return static_cast<float>(value) / FIXED_ONE;
    }
    
    double toDouble() const {
        return static_cast<double>(value) / FIXED_ONE;
    }
    
    int16_t getRaw() const { return value; }
    void setRaw(int16_t val) { value = val; }
    
    FixedPoint16 operator+(const FixedPoint16& other) const {
        FixedPoint16 result;
        result.value = value + other.value;
        return result;
    }
    
    FixedPoint16 operator-(const FixedPoint16& other) const {
        FixedPoint16 result;
        result.value = value - other.value;
        return result;
    }
    
    FixedPoint16 operator>>(int shift) const {
        FixedPoint16 result;
        result.value = value >> shift;
        return result;
    }
    
    bool operator>=(int) const { return value >= 0; }
    
    bool operator<(double threshold) const { 
        return std::abs(toDouble()) < threshold; 
    }
    
    bool hasConverged() const {
        return std::abs(toDouble()) < CORDICConfig::CONVERGENCE_THRESHOLD;
    }
};

//==============================================================================
// ESTRUCTURAS DE DATOS
//==============================================================================

struct PreprocessResult {
    FixedPoint16 mapped_input;
    int reduction_factor;
    bool mapping_applied;
    float original_input;
    
    PreprocessResult() : reduction_factor(0), mapping_applied(false), original_input(0.0f) {}
};

struct CORDICState {
    FixedPoint16 X, Y, Z;
    int iteration_count;
    bool converged;
    
    CORDICState() : iteration_count(0), converged(false) {}
    CORDICState(FixedPoint16 x, FixedPoint16 y, FixedPoint16 z) 
        : X(x), Y(y), Z(z), iteration_count(0), converged(false) {}
};

struct IterationResult {
    CORDICState final_state;
    std::vector<int> selected_angles;
    int iterations_used;
    bool converged_successfully;
    
    IterationResult() : iterations_used(0), converged_successfully(false) {}
};

struct PostprocessResult {
    float exponential_value;
    float cosh_value;
    float sinh_value;
    float scaling_factor;
    float relative_error;
    
    PostprocessResult() : exponential_value(0.0f), cosh_value(0.0f), 
                         sinh_value(0.0f), scaling_factor(1.0f), relative_error(0.0f) {}
};

struct AngleTableEntry {
    double angle;
    double tanh_value;
    int shift_amount;
    FixedPoint16 fixed_angle;
    
    AngleTableEntry() : angle(0.0), tanh_value(0.0), shift_amount(0) {}
    
    AngleTableEntry(int k) : shift_amount(k) {
        tanh_value = std::pow(2.0, -k);
        angle = std::atanh(tanh_value);
        fixed_angle = FixedPoint16(angle);
    }
};

#endif // CORDIC_TYPES_H
