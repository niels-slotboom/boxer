file(GLOB_RECURSE SOURCES_BOXER "*.cpp")
file(GLOB_RECURSE HEADERS_BOXER "*.hpp" "*.h")

# Qt flags
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

add_library(boxer STATIC ${SOURCES_BOXER} ${HEADERS_BOXER})

target_include_directories(boxer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(boxer PUBLIC amrex Qt6::Widgets Qt6::OpenGLWidgets)