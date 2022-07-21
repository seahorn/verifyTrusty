add_executable(storage_ipc_indirect_handlers
  ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
  indirect_handlers_harness.c)
target_include_directories(storage_ipc_indirect_handlers 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
  )
sea_link_libraries(storage_ipc_indirect_handlers sea_storage_ipc_proofs.ir)
sea_overlink_libraries(storage_ipc_indirect_handlers realloc_override.ir)
sea_overlink_libraries(storage_ipc_indirect_handlers fprintf_override.ir)
sea_overlink_libraries(storage_ipc_indirect_handlers ipc_loop_override.ir)
sea_attach_bc_link(storage_ipc_indirect_handlers)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(storage_ipc_indirect_handlers)