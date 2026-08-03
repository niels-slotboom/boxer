## Usage
It is easiest to pull in this repository using CMake. An example top-level `CMakeLists.txt` is provided below:
```cmake
cmake_minimum_required(VERSION 3.15)

project(BoxerApp VERSION 0.1.0 LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    Boxer
    GIT_REPOSITORY https://github.com/niels-slotboom/boxer.git
    GIT_TAG        main
)

set(AMREX_SPACEDIM 3 CACHE STRING "AMReX Space Dimension")
set(AMREX_GPU_BACKEND CUDA CACHE STRING "AMReX GPU Backend")
set(AMREX_ENABLE_TESTS OFF CACHE BOOL "Disable AMReX Tests")

FetchContent_MakeAvailable(Boxer)

add_executable(boxer-app src/main.cpp)

target_link_libraries(boxer-app PRIVATE boxer)
```
The API of `boxer` is provided by `Boxer.hpp`, which contains `boxer::show(const amrex::AmrCore& container, int ngrow)`. 
This function may be used as in the following example `main.cpp`:
```cpp
#include "AMRContainer.hpp"
#include <Boxer.hpp>

int main(int argc, char* argv[]) { // Initialize AMReX (handles MPI setup, GPU device selection, etc.)
    amrex::Initialize(argc, argv);
    {
        // Set up geometry and AMR parameters geom, amr_info, ngrow...

        AMRContainer amr(geom, amr_info, 1, ngrow);
        amr.InitFromScratch(0.0);
        boxer::show(amr, ngrow); // call to Boxer API to open AMR structure visualisation window
    }
    // Clean up resources
    amrex::Finalize();
    return 0;
}
```

## License

Boxer is licensed under the [MIT License](LICENSE.txt).

### Dependencies
* **Qt 6** (LGPLv3)
* **AMReX** (BSD 3-Clause)
