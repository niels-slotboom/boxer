include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES_BOXER
    Boxer.cpp
    MainWindow.cpp 
    VisualisationWidget_BoxBuffer.cpp
    VisualisationWidget_Camera.cpp
    VisualisationWidget_OpenGL.cpp
    AmrMeshWrapper.cpp
)

set(PRIVATE_HEADERS_BOXER
    MainWindow.hpp
    VisualisationWidget.hpp
)

set(PUBLIC_HEADERS_BOXER
    include/Boxer/Boxer.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/include/Boxer/boxer_export.hpp
)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

# 1. Change target type to SHARED
add_library(boxer SHARED 
    ${SOURCES_BOXER}
    ${PRIVATE_HEADERS_BOXER}
    include/Boxer/Boxer.hpp
)

add_library(Boxer::boxer ALIAS boxer)

# 2. Hide all internal symbols by default (isolates AMReX completely)
set_target_properties(boxer PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    PUBLIC_HEADER "${PUBLIC_HEADERS_BOXER}"
)

# 3. Generate export macro header for Boxer's public interface
generate_export_header(boxer
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/include/Boxer/boxer_export.hpp"
    EXPORT_MACRO_NAME BOXER_EXPORT
)

# 4. Expose include paths (including binary dir for generated boxer_export.hpp)
target_include_directories(boxer 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(boxer PUBLIC cxx_std_20)

# 5. Link amrex PRIVATELY so it is contained within libboxer.so
target_link_libraries(boxer 
    PUBLIC 
        Qt6::Widgets 
        Qt6::OpenGLWidgets
    PRIVATE 
        amrex
)

# --- Installation & Export Rules ---

set(BOXER_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Boxer")

install(TARGETS boxer
    EXPORT BoxerTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Boxer
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/BoxerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/BoxerConfig.cmake"
    INSTALL_DESTINATION "${BOXER_CMAKE_CONFIG_DESTINATION}"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/BoxerConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/BoxerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/BoxerConfigVersion.cmake"
    DESTINATION "${BOXER_CMAKE_CONFIG_DESTINATION}"
)

install(EXPORT BoxerTargets
    FILE BoxerTargets.cmake
    NAMESPACE Boxer::
    DESTINATION "${BOXER_CMAKE_CONFIG_DESTINATION}"
)