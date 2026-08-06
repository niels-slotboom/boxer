## Prerequisites

* C++20 compatible compiler
* CMake (3.20+)
* Qt6 (`Widgets`, `OpenGLWidgets`)

---

## Installation

```bash
git clone https://github.com/niels-slotboom/boxer.git
cd boxer

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Install to system default (/usr/local)
sudo cmake --install build

# Alternatively, install to a custom directory:
# cmake --install build --prefix /path/to/install

```

---

## Downstream Integration

### CMake Setup

```cmake
find_package(Boxer REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE Boxer::boxer)

```

If installed to a custom prefix, pass `-DCMAKE_PREFIX_PATH=/path/to/install` when configuring downstream projects.

---

## Minimal Example

```cpp
#include <amrex/amrex.hpp>
#include <Boxer/Boxer.hpp>

int main(int argc, char* argv[]) {
    amrex::Initialize(argc, argv);
    {
        // Your AMReX mesh instance
        amrex::AmrMesh mesh;

        // Pass to Boxer visualization
        boxer::show(mesh);
    }
    amrex::Finalize();
    return 0;
}

```

## License

Boxer is licensed under the [MIT License](LICENSE.txt).

### Dependencies
* **Qt 6** (LGPLv3)
* **AMReX** (BSD 3-Clause)
