file(GLOB_RECURSE SOURCES_TEST_MAIN CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE HEADERS_TEST_MAIN CONFIGURE_DEPENDS "*.h" "*.hpp")

if(SOURCES_TEST_MAIN)
    set_source_files_properties(${SOURCES_TEST_MAIN} PROPERTIES LANGUAGE CUDA)
endif()

add_executable(test-main ${SOURCES_TEST_MAIN} ${HEADERS_TEST_MAIN})

if(SOURCES_TEST_MAIN)
    set_source_files_properties(${SOURCES_TEST_MAIN} PROPERTIES LANGUAGE CUDA)
endif()

target_link_libraries(test-main PRIVATE amrex boxer)