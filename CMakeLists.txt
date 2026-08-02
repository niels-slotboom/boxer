cmake_minimum_required(VERSION 3.15)

project(Boxer CXX CUDA)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_BUILD_TYPE Release)

# Set compiler flags/options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# fetch amrex
include(FetchContent)
FetchContent_Declare(
    amrex
    GIT_REPOSITORY https://github.com/AMReX-Codes/amrex.git
    GIT_TAG        development
)

# Set AMReX options before making available
set(AMREX_SPACEDIM 3 CACHE STRING "" FORCE)
set(AMREX_GPU_BACKEND CUDA CACHE STRING "" FORCE) # Since you use CUDA
set(AMREX_ENABLE_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(amrex)

add_subdirectory(src/boxer)
add_subdirectory(src/test-main)