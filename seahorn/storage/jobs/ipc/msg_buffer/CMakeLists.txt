add_executable(storage_ipc_msg_buffer
  ${IPC_SRC}
  msg_buffer_harness.c)

target_include_directories(storage_ipc_msg_buffer 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
  )
sea_link_libraries(storage_ipc_msg_buffer sea_storage_ipc_proofs.ir)
sea_overlink_libraries(storage_ipc_msg_buffer realloc_override.ir)
sea_overlink_libraries(storage_ipc_msg_buffer fprintf_override.ir)
sea_overlink_libraries(storage_ipc_msg_buffer ipc_loop_override.ir)
sea_attach_bc_link(storage_ipc_msg_buffer)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(storage_ipc_msg_buffer)