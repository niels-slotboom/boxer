file(GLOB_RECURSE SOURCES_TEST_MAIN "*.cpp")
file(GLOB_RECURSE HEADERS_TEST_MAIN "*.h" "*.hpp")

add_executable(test-main ${SOURCES_TEST_MAIN} ${HEADERS_TEST_MAIN})

target_link_libraries(test-main PRIVATE amrex boxer)