add_executable(port_wait_pf main.c)
sea_link_libraries(port_wait_pf sea_libc_trusty.ir)

sea_attach_bc(port_wait_pf)
sea_add_unsat_test(port_wait_pf)
