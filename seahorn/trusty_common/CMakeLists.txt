
configure_file(include/config.h.in ${PROJECT_BINARY_DIR}/include/config.h)
add_subdirectory(lib)
add_subdirectory(stubs)