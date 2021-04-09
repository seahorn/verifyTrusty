set(EXTERNAL ${TRUSTY_ROOT}/trusty/user/base/lib/libc-trusty)
add_library(
  sea_libc_trusty
  ${EXTERNAL}/ipc.c
  sea_handle_table.c
  trusty_syscalls_impl.c
)

target_include_directories(sea_libc_trusty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

sea_attach_bc(sea_libc_trusty)

add_subdirectory(proof)
