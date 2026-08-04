# Enable Qt automatic meta-object code generation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 1. Explicitly list source and header files
set(SOURCES_BOXER
    Boxer.cpp
    MainWindow.cpp    
    VisualisationWidget_BoxBuffer.cpp
    VisualisationWidget_Camera.cpp
    VisualisationWidget_OpenGL.cpp
)

set(CUDA_SOURCES_BOXER
    AmrMeshWrapper.cpp
)

set(HEADERS_BOXER
    Boxer.hpp
    MainWindow.hpp
    VisualisationWidget.hpp
)


set_source_files_properties(${CUDA_SOURCES_BOXER} PROPERTIES LANGUAGE CUDA) 


# 2. Find Qt6 components required by Boxer
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

# 3. Define static library
add_library(boxer STATIC 
    ${SOURCES_BOXER}
    ${CUDA_SOURCES_BOXER}
    ${HEADERS_BOXER}
)

# 4. Create alias target for external parent projects using FetchContent
add_library(Boxer::boxer ALIAS boxer)

set_target_properties(boxer PROPERTIES
    CUDA_STANDARD 20
    CUDA_STANDARD_REQUIRED ON
    CUDA_SEPARABLE_COMPILATION OFF
    CUDA_RESOLVE_DEVICE_SYMBOLS OFF
)

# 5. Export include directories safely using generator expressions
target_include_directories(boxer PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# 6. Propagate C++20 standard requirements down to consumers
target_compile_features(boxer PUBLIC cxx_std_20)

# 7. Link required dependencies
target_link_libraries(boxer 
    PUBLIC 
        amrex
        Qt6::Widgets 
        Qt6::OpenGLWidgets
)