cmake_minimum_required(VERSION 3.15)

# 1. Guard top-level vs fetch/add_subdirectory checks
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(BOXER_IS_TOP_LEVEL ON)
else()
    set(BOXER_IS_TOP_LEVEL OFF)
endif()

# 2. Only require CUDA if top-level, or check if language is enabled
project(Boxer VERSION 0.1.0 LANGUAGES CXX CUDA)

# 3. Only set build paths and defaults when built standalone
if(BOXER_IS_TOP_LEVEL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()
endif()

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 4. Include AMReX (avoid FORCE so parent project can override cache)
include(FetchContent)
FetchContent_Declare(
    amrex
    GIT_REPOSITORY https://github.com/AMReX-Codes/amrex.git
    GIT_TAG        development
)

set(AMREX_SPACEDIM 3 CACHE STRING "AMReX Space Dimension")
set(AMREX_GPU_BACKEND CUDA CACHE STRING "AMReX GPU Backend")
set(AMREX_ENABLE_TESTS OFF CACHE BOOL "Disable AMReX Tests")

FetchContent_MakeAvailable(amrex)

add_subdirectory(src/boxer)

# 5. Only build test executables when standalone
if(BOXER_IS_TOP_LEVEL)
    add_subdirectory(src/test-main)
endif()