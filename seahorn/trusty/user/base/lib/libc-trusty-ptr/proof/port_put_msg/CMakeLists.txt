add_executable(port_put_msg_pf main.c)
sea_link_libraries(port_put_msg_pf sea_libc_trusty.ir)

sea_attach_bc(port_put_msg_pf)
configure_file(sea.yaml sea.yaml @ONLY)
sea_add_unsat_test(port_put_msg_pf)
