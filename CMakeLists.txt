cmake_minimum_required(VERSION 3.25)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(BOXER_IS_TOP_LEVEL ON)
else()
    set(BOXER_IS_TOP_LEVEL OFF)
endif()

project(Boxer VERSION 0.1.0 LANGUAGES CXX CUDA)

if(BOXER_IS_TOP_LEVEL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    amrex
    GIT_REPOSITORY https://github.com/AMReX-Codes/amrex.git
    GIT_TAG        development
)

# AMReX Configuration Options
set(AMReX_SPACEDIM 3 CACHE STRING "AMReX Space Dimension")
set(AMReX_CUDA ON CACHE BOOL "Enable CUDA in AMReX")
set(AMReX_GPU_BACKEND CUDA CACHE STRING "AMReX GPU Backend")
set(AMReX_ENABLE_TESTS OFF CACHE BOOL "Disable AMReX Tests")

# Tell AMReX to bypass its internal compiler/flag setup
set(AMReX_SETUP_COMPILER_FLAGS OFF CACHE BOOL "Disable internal AMReX compiler flag setup")

FetchContent_MakeAvailable(amrex)

add_subdirectory(src/boxer)

if(BOXER_IS_TOP_LEVEL)
   # add_subdirectory(src/test-main)
endif()