add_executable(port_connect_pf main.c)
sea_link_libraries(port_connect_pf sea_libc_trusty.ir)

sea_attach_bc(port_connect_pf)
sea_add_unsat_test(port_connect_pf)
