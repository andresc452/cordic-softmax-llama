cmake_minimum_required(VERSION 3.15)
project(cordic_softmax_llama 
    VERSION 0.1.0
    DESCRIPTION "CORDIC Softmax implementation for llama.cpp with FPGA acceleration"
    LANGUAGES CXX
)

# ============================================================================
# CONFIGURACIÓN
# ============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ============================================================================
# RUTAS
# ============================================================================

set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_SOURCE_DIR_SRC "${PROJECT_SOURCE_DIR}/src")
set(PROJECT_TEST_DIR "${PROJECT_SOURCE_DIR}/tests")

# ============================================================================
# LIBRERÍA CORDIC
# ============================================================================

set(CORDIC_HEADERS
    ${PROJECT_INCLUDE_DIR}/cordic_types.h
    ${PROJECT_INCLUDE_DIR}/cordic_preprocessor.h
    ${PROJECT_INCLUDE_DIR}/cordic_iterator.h
    ${PROJECT_INCLUDE_DIR}/cordic_postprocessor.h
)

set(CORDIC_SOURCES
    ${PROJECT_SOURCE_DIR_SRC}/cordic_preprocessor.cpp
    ${PROJECT_SOURCE_DIR_SRC}/cordic_iterator.cpp
    ${PROJECT_SOURCE_DIR_SRC}/cordic_postprocessor.cpp
)

# Verificar archivos
foreach(header ${CORDIC_HEADERS})
    if(NOT EXISTS ${header})
        message(FATAL_ERROR "Header not found: ${header}")
    endif()
endforeach()

foreach(source ${CORDIC_SOURCES})
    if(NOT EXISTS ${source})
        message(FATAL_ERROR "Source not found: ${source}")
    endif()
endforeach()

# Crear librería
add_library(cordic_static STATIC ${CORDIC_SOURCES} ${CORDIC_HEADERS})

target_include_directories(cordic_static
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(cordic_static PROPERTIES 
    OUTPUT_NAME cordic
    POSITION_INDEPENDENT_CODE ON
)

# ============================================================================
# TESTS
# ============================================================================

enable_testing()

add_executable(test_types ${PROJECT_TEST_DIR}/test_types.cpp)
target_link_libraries(test_types PRIVATE cordic_static)
add_test(NAME test_types COMMAND test_types)

add_executable(test_preprocessor ${PROJECT_TEST_DIR}/test_preprocessor.cpp)
target_link_libraries(test_preprocessor PRIVATE cordic_static)
add_test(NAME test_preprocessor COMMAND test_preprocessor)

add_executable(test_iterator ${PROJECT_TEST_DIR}/test_iterator.cpp)
target_link_libraries(test_iterator PRIVATE cordic_static)
add_test(NAME test_iterator COMMAND test_iterator)

# Test 4: Postprocessor (NUEVO)
add_executable(test_postprocessor ${PROJECT_TEST_DIR}/test_postprocessor.cpp)
target_link_libraries(test_postprocessor PRIVATE cordic_static)
add_test(NAME test_postprocessor COMMAND test_postprocessor)

# ============================================================================
# CUSTOM TARGETS
# ============================================================================

add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_types test_preprocessor test_iterator test_postprocessor
    COMMENT "Running all tests..."
)

# ============================================================================
# SUMMARY
# ============================================================================

message(STATUS "============================================")
message(STATUS "CORDIC Softmax Configuration Summary")
message(STATUS "============================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "============================================")
