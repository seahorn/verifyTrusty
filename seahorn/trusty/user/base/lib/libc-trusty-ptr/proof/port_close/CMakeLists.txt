add_executable(port_close_pf main.c)
sea_link_libraries(port_close_pf sea_libc_trusty.ir)

sea_attach_bc(port_close_pf)
sea_add_unsat_test(port_close_pf)
