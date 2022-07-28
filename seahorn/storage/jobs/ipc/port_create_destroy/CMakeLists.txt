# We verify a modified copy of ipc.c since the original
# casts between a handle_t and int.
# This breaks the original spec that handle_t is a 32-bit opaque number
# and hence breaks our impl of type(handle_t) == 32 bit pointer.
add_executable(storage_ipc_port_create_destroy
  ${IPC_SRC}
  port_create_destroy_harness.c)
target_include_directories(storage_ipc_port_create_destroy
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
  )
sea_link_libraries(storage_ipc_port_create_destroy sea_storage_ipc_proofs.ir)
sea_overlink_libraries(storage_ipc_port_create_destroy realloc_override.ir)
sea_overlink_libraries(storage_ipc_port_create_destroy fprintf_override.ir)
sea_overlink_libraries(storage_ipc_port_create_destroy ipc_loop_override.ir)
sea_attach_bc_link(storage_ipc_port_create_destroy)
sea_add_unsat_test(storage_ipc_port_create_destroy)