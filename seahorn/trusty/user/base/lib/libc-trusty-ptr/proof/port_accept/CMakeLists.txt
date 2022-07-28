add_executable(port_accept_pf main.c)
sea_link_libraries(port_accept_pf sea_libc_trusty.ir)

sea_attach_bc(port_accept_pf)
sea_add_unsat_test(port_accept_pf)
