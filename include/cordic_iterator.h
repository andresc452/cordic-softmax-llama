/**
 * @file cordic_iterator.h
 * @brief Módulo de iteración CORDIC - Núcleo del algoritmo
 * 
 * FUNCIÓN: Ejecutar rotaciones CORDIC con selección greedy de ángulos
 * 
 * ALGORITMO:
 * 1. Selección greedy: elegir ángulo más grande ≤ |Z_residual|
 * 2. Rotación hiperbólica: actualizar X, Y, Z usando shifts
 * 3. Repetir hasta convergencia o máximo de iteraciones
 */

#ifndef CORDIC_ITERATOR_H
#define CORDIC_ITERATOR_H

#include "cordic_types.h"
#include <vector>

/**
 * @class AngleTable
 * @brief Tabla de ángulos elementales para CORDIC hiperbólico
 * 
 * Contiene ángulos α_k = arctanh(2^-k) precomputados
 */
class AngleTable {
private:
    std::vector<AngleTableEntry> table;
    static constexpr int TABLE_SIZE = 15;
    
public:
    AngleTable();
    
    /**
     * @brief Construye la tabla de ángulos α_k = arctanh(2^-k)
     */
    void buildTable();
    
    /**
     * @brief Obtiene entrada de la tabla
     * @param index Índice (base 1)
     */
    const AngleTableEntry& getEntry(int index) const;
    
    int size() const { return static_cast<int>(table.size()); }
    bool hasIndex(int index) const;
    void printTable() const;
};

/**
 * @class CORDICIterator
 * @brief Ejecuta las iteraciones CORDIC con algoritmo greedy
 */
class CORDICIterator {
private:
    AngleTable angle_table;
    
public:
    CORDICIterator();
    
    /**
     * @brief Ejecuta las iteraciones CORDIC completas
     * 
     * ALGORITMO PRINCIPAL:
     * 1. Verificar convergencia de Z
     * 2. Seleccionar ángulo óptimo (greedy)
     * 3. Ejecutar rotación hiperbólica
     * 4. Repetir hasta convergencia
     * 
     * @param initial_state Estado inicial de variables CORDIC
     * @param enable_debug Mostrar progreso de iteraciones
     * @return Resultado completo de las iteraciones
     */
    IterationResult performIterations(const CORDICState& initial_state, 
                                     bool enable_debug = false);
    
    /**
     * @brief Obtiene referencia a la tabla de ángulos (para debugging)
     */
    const AngleTable& getAngleTable() const { return angle_table; }

private:
    /**
     * @brief Selección greedy del ángulo óptimo
     * 
     * ESTRATEGIA: Encontrar el ángulo más grande de la tabla
     * que sea ≤ |Z_residual|
     * 
     * @param z_residual Valor Z actual
     * @return Índice del ángulo seleccionado (0 si convergió)
     */
    int selectGreedyAngle(const FixedPoint16& z_residual);
    
    /**
     * @brief Ejecuta un paso de rotación CORDIC
     * 
     * ECUACIONES CORDIC HIPERBÓLICAS:
     * X_{n+1} = X_n + s_n × 2^(-k) × Y_n
     * Y_{n+1} = Y_n + s_n × 2^(-k) × X_n
     * Z_{n+1} = Z_n - s_n × α_k
     * 
     * donde s_n = sign(Z_n) y k es el shift_amount
     * 
     * @param current_state Estado actual
     * @param angle_idx Índice del ángulo seleccionado
     * @return Nuevo estado después de la rotación
     */
    CORDICState executeRotationStep(const CORDICState& current_state, 
                                   int angle_idx);
};

#endif // CORDIC_ITERATOR_H
