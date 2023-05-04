# We verify a modified copy of ipc.c since the original
# casts between a handle_t and int.
# This breaks the original spec that handle_t is a 32-bit opaque number
# and hence breaks our impl of type(handle_t) == 32 bit pointer.
add_executable(storage_ipc_unit
  ${STORAGE_IPC_SRC}
  ipc_unit_harness.c)
target_include_directories(storage_ipc_unit PRIVATE
  ${TRUSTY_ROOT}/trusty/user/app/storage)
sea_link_libraries(storage_ipc_unit sea_storage_ipc_proofs.ir)
sea_link_libraries(storage_ipc_unit sea_libc.ir)
sea_overlink_libraries(storage_ipc_unit fprintf_override.ir)
sea_overlink_libraries(storage_ipc_unit ipc_syscalls_mock_override.ir)
#sea_overlink_libraries(storage_ipc_unit ipc_loop_override.ir)
sea_attach_bc_link(storage_ipc_unit)
sea_add_unsat_test(storage_ipc_unit)
