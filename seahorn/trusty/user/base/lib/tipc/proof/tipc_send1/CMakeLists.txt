add_executable(tipc_send1_pf main.c)
sea_link_libraries(tipc_send1_pf sea_trusty_tipc.ir)

sea_attach_bc(tipc_send1_pf)
sea_add_unsat_test(tipc_send1_pf)