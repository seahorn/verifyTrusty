target_sources(sea_libc_trusty PUBLIC
  sea_handle_table.c
  trusty_syscalls_impl.c
  )

target_include_directories(sea_libc_trusty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

sea_attach_bc(sea_libc_trusty)

add_subdirectory(proof)