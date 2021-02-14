
add_executable(storage_ipc_port_create_destroy
ipc_orig.c
ipc_harness.c)
sea_overlink_libraries(storage_ipc_port_create_destroy trusty_msg.ir)
sea_overlink_libraries(storage_ipc_port_create_destroy realloc_override.ir)
sea_attach_bc_link(storage_ipc_port_create_destroy)
if ("${TRUSTY_TARGET}" MATCHES "^.*64.*$" )
    configure_file(sea.yaml sea.yaml @ONLY)
endif()
sea_add_unsat_test(storage_ipc_port_create_destroy)