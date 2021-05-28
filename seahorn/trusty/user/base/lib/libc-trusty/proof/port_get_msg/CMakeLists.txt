add_executable(port_get_msg_pf main.c)
sea_link_libraries(port_get_msg_pf sea_libc_trusty.ir)

sea_attach_bc(port_get_msg_pf)
sea_add_unsat_test(port_get_msg_pf)
