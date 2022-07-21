include_directories(include)

set(EXTERNAL ${TRUSTY_ROOT}/trusty/user/base/lib/tipc)
add_library(
  sea_trusty_tipc
  ${EXTERNAL}/tipc.c
  trusty_ipc_impl.c
)

target_include_directories(sea_trusty_tipc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

sea_attach_bc(sea_trusty_tipc)

add_subdirectory(proof)