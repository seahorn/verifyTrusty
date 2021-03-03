
add_executable(storage_ipc_msg_buffer
${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c
main.c)
sea_overlink_libraries(storage_ipc_msg_buffer trusty_msg.ir)
sea_overlink_libraries(storage_ipc_msg_buffer realloc_override.ir)
sea_overlink_libraries(storage_ipc_msg_buffer fprintf_override.ir)
sea_attach_bc_link(storage_ipc_msg_buffer)
if ("${TRUSTY_TARGET}" MATCHES "^.*64.*$" )
    configure_file(sea.yaml sea.yaml @ONLY)
endif()
sea_add_unsat_test(storage_ipc_msg_buffer)