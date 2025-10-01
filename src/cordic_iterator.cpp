/**
 * @file cordic_iterator.cpp
 * @brief Implementación del iterador CORDIC
 */

#include "cordic_iterator.h"
#include <iostream>
#include <iomanip>
#include <cmath>

//==============================================================================
// IMPLEMENTACIÓN AngleTable
//==============================================================================

AngleTable::AngleTable() {
    buildTable();
}

void AngleTable::buildTable() {
    table.clear();
    for (int k = 1; k <= TABLE_SIZE; k++) {
        table.emplace_back(k);
    }
}

const AngleTableEntry& AngleTable::getEntry(int index) const {
    if (index < 1 || index > static_cast<int>(table.size())) {
        throw std::out_of_range("Angle table index out of range");
    }
    return table[index - 1];
}

bool AngleTable::hasIndex(int index) const {
    return index >= 1 && index <= static_cast<int>(table.size());
}

void AngleTable::printTable() const {
    std::cout << "\n=== TABLA DE ÁNGULOS CORDIC HIPERBÓLICO ===" << std::endl;
    std::cout << "k\t| tanh(αₖ)\t| αₖ\t\t| Shift\t| Punto Fijo" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    for (size_t i = 0; i < table.size(); i++) {
        const auto& entry = table[i];
        std::cout << (i+1) << "\t| " 
                  << std::fixed << std::setprecision(6) << entry.tanh_value << "\t| "
                  << std::setprecision(6) << entry.angle << "\t| "
                  << entry.shift_amount << "\t| "
                  << entry.fixed_angle.getRaw() << std::endl;
    }
}

//==============================================================================
// IMPLEMENTACIÓN CORDICIterator
//==============================================================================

CORDICIterator::CORDICIterator() {
    // La tabla se construye automáticamente
}

IterationResult CORDICIterator::performIterations(const CORDICState& initial_state, 
                                                  bool enable_debug) {
    IterationResult result;
    CORDICState current_state = initial_state;
    
    if (enable_debug) {
        std::cout << "\n=== ITERACIONES CORDIC ===" << std::endl;
        std::cout << "Estado inicial:" << std::endl;
        std::cout << "  X₀ = " << current_state.X.toFloat() << std::endl;
        std::cout << "  Y₀ = " << current_state.Y.toFloat() << std::endl;
        std::cout << "  Z₀ = " << current_state.Z.toFloat() << std::endl;
        std::cout << "\nIter\tÍndice\tÁngulo\t\tZ_residual\tX\t\tY" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
    }
    
    for (int iter = 0; iter < CORDICConfig::MAX_ITERATIONS; iter++) {
        if (current_state.Z.hasConverged()) {
            if (enable_debug) {
                std::cout << "✓ Convergencia alcanzada en iteración " << iter << std::endl;
            }
            current_state.converged = true;
            result.converged_successfully = true;
            break;
        }
        
        int selected_angle_idx = selectGreedyAngle(current_state.Z);
        
        if (selected_angle_idx <= 0) {
            if (enable_debug) {
                std::cout << "⚠ No se encontró ángulo válido" << std::endl;
            }
            break;
        }
        
        result.selected_angles.push_back(selected_angle_idx);
        CORDICState next_state = executeRotationStep(current_state, selected_angle_idx);
        
        if (enable_debug) {
            const auto& angle_entry = angle_table.getEntry(selected_angle_idx);
            std::cout << iter << "\t" << selected_angle_idx << "\t"
                      << std::fixed << std::setprecision(6) << angle_entry.angle << "\t"
                      << std::setprecision(6) << next_state.Z.toFloat() << "\t"
                      << std::setprecision(6) << next_state.X.toFloat() << "\t"
                      << std::setprecision(6) << next_state.Y.toFloat() << std::endl;
        }
        
        current_state = next_state;
        current_state.iteration_count = iter + 1;
        result.iterations_used++;
    }
    
    result.final_state = current_state;
    
    if (!result.converged_successfully && enable_debug) {
        std::cout << "⚠ Máximo de iteraciones alcanzado" << std::endl;
        std::cout << "  Z_residual: " << current_state.Z.toFloat() << std::endl;
    }
    
    return result;
}

int CORDICIterator::selectGreedyAngle(const FixedPoint16& z_residual) {
    if (z_residual.hasConverged()) {
        return 0;
    }
    
    float abs_z = std::abs(z_residual.toFloat());
    
    for (int i = 1; i <= angle_table.size(); i++) {
        const auto& entry = angle_table.getEntry(i);
        if (entry.angle <= abs_z + 1e-6) {
            return i;
        }
    }
    
    return angle_table.size();
}

CORDICState CORDICIterator::executeRotationStep(const CORDICState& current_state, 
                                               int angle_idx) {
    CORDICState next_state = current_state;
    
    if (!angle_table.hasIndex(angle_idx)) {
        return next_state;
    }
    
    const auto& angle_entry = angle_table.getEntry(angle_idx);
    int rotation_direction = (current_state.Z >= 0) ? 1 : -1;
    
    FixedPoint16 shifted_Y = current_state.Y >> angle_entry.shift_amount;
    FixedPoint16 shifted_X = current_state.X >> angle_entry.shift_amount;
    
    FixedPoint16 delta_X;
    delta_X.setRaw(rotation_direction * shifted_Y.getRaw());
    next_state.X = current_state.X + delta_X;
    
    FixedPoint16 delta_Y;
    delta_Y.setRaw(rotation_direction * shifted_X.getRaw());
    next_state.Y = current_state.Y + delta_Y;
    
    FixedPoint16 delta_Z;
    delta_Z.setRaw(rotation_direction * angle_entry.fixed_angle.getRaw());
    next_state.Z = current_state.Z - delta_Z;
    
    return next_state;
}
