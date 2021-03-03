
add_executable(storage_ipc_indirect_handlers
${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
main.c)
sea_overlink_libraries(storage_ipc_indirect_handlers trusty_msg.ir)
sea_overlink_libraries(storage_ipc_indirect_handlers realloc_override.ir)
sea_overlink_libraries(storage_ipc_indirect_handlers fprintf_override.ir)
sea_attach_bc_link(storage_ipc_indirect_handlers)
if ("${TRUSTY_TARGET}" MATCHES "^.*64.*$" )
    configure_file(sea.yaml sea.yaml @ONLY)
endif()
sea_add_unsat_test(storage_ipc_indirect_handlers)