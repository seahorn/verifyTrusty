
add_executable(storage_ipc_msg_buffer
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  main.c)
target_include_directories(storage_ipc_msg_buffer
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage)
sea_overlink_libraries(storage_ipc_msg_buffer trusty_api_override.ir)
sea_overlink_libraries(storage_ipc_msg_buffer realloc_override.ir)
sea_overlink_libraries(storage_ipc_msg_buffer fprintf_override.ir)
sea_attach_bc_link(storage_ipc_msg_buffer)
sea_add_unsat_test(storage_ipc_msg_buffer)
