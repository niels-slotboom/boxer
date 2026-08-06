cmake_minimum_required(VERSION 3.25)

project(Boxer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    amrex
    GIT_REPOSITORY https://github.com/AMReX-Codes/amrex.git
    GIT_TAG        development
)

set(AMReX_SPACEDIM 3 CACHE STRING "AMReX Space Dimension")
set(AMReX_ENABLE_TESTS OFF CACHE BOOL "Disable AMReX Tests")
set(AMReX_SETUP_COMPILER_FLAGS OFF CACHE BOOL "Disable internal AMReX compiler flag setup")

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FetchContent_MakeAvailable(amrex)

add_subdirectory(src/boxer)