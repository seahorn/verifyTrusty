list(APPEND JOB_LISTS storage_ipc_indirect_handlers 
storage_ipc_msg_buffer storage_ipc_port_create_destroy)

foreach(JOB ${JOB_LISTS})
  add_executable(${JOB}
    ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
    ${JOB}.c)
  target_include_directories(${JOB} 
    PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage
    )
  sea_link_libraries(${JOB} sea_storage_ipc_proofs.ir)
  sea_overlink_libraries(${JOB} trusty_api_override.ir)
  sea_overlink_libraries(${JOB} realloc_override.ir)
  sea_overlink_libraries(${JOB} fprintf_override.ir)
  sea_overlink_libraries(${JOB} ipc_loop_override.ir)
  sea_attach_bc_link(${JOB})
  sea_add_unsat_test(${JOB})
endforeach()