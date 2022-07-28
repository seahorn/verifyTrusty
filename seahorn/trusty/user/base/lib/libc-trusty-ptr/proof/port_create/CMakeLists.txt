add_executable(port_create_pf main.c)
sea_link_libraries(port_create_pf sea_libc_trusty.ir)

sea_attach_bc(port_create_pf)
sea_add_unsat_test(port_create_pf)
