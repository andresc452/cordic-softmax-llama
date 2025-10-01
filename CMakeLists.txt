cmake_minimum_required(VERSION 3.15)
project(cordic_softmax_llama 
    VERSION 0.1.0
    DESCRIPTION "CORDIC Softmax implementation for llama.cpp with FPGA acceleration"
    LANGUAGES CXX
)

# ============================================================================
# CONFIGURACIÓN DEL PROYECTO
# ============================================================================

# Estándar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flags de compilación
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

# Configuración por defecto: Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

# ============================================================================
# DIRECTORIOS
# ============================================================================

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)

include_directories(${INCLUDE_DIR})

# ============================================================================
# LIBRERÍA CORDIC (STATIC)
# ============================================================================

set(CORDIC_SOURCES
    ${SRC_DIR}/cordic_preprocessor.cpp
    # Agregar más archivos aquí cuando se creen:
    # ${SRC_DIR}/cordic_iterator.cpp
    # ${SRC_DIR}/cordic_postprocessor.cpp
    # ${SRC_DIR}/cordic_softmax.cpp
)

set(CORDIC_HEADERS
    ${INCLUDE_DIR}/cordic_types.h
    ${INCLUDE_DIR}/cordic_preprocessor.h
    # Agregar más headers aquí:
    # ${INCLUDE_DIR}/cordic_iterator.h
    # ${INCLUDE_DIR}/cordic_postprocessor.h
    # ${INCLUDE_DIR}/cordic_softmax.h
)

add_library(cordic_static STATIC ${CORDIC_SOURCES})
target_include_directories(cordic_static PUBLIC ${INCLUDE_DIR})
set_target_properties(cordic_static PROPERTIES OUTPUT_NAME cordic)

# ============================================================================
# TESTS (EXECUTABLES)
# ============================================================================

enable_testing()

# Test 1: cordic_types
add_executable(test_types ${TESTS_DIR}/test_types.cpp)
target_link_libraries(test_types cordic_static)
add_test(NAME test_types COMMAND test_types)

# Test 2: cordic_preprocessor
add_executable(test_preprocessor 
    ${TESTS_DIR}/test_preprocessor.cpp
)
target_link_libraries(test_preprocessor cordic_static)
add_test(NAME test_preprocessor COMMAND test_preprocessor)

# ============================================================================
# TARGETS PERSONALIZADOS
# ============================================================================

# Target para ejecutar todos los tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_types test_preprocessor
    COMMENT "Running all tests..."
)

# Target para limpiar completamente
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Removing all build files..."
)

# ============================================================================
# INFORMACIÓN DE BUILD
# ============================================================================

message(STATUS "============================================")
message(STATUS "CORDIC Softmax Configuration Summary")
message(STATUS "============================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "============================================")
