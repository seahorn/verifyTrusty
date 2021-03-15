add_executable(gatekeeper_ipc
${TRUSTY_ROOT}/trusty/user/app/gatekeeper/trusty_gatekeeper.cpp
gatekeeper_ipc_harness.cpp)
target_compile_definitions(gatekeeper_ipc PRIVATE 
  GATEKEEPER_STORAGE_PORT=STORAGE_CLIENT_TP_PORT)
target_include_directories(gatekeeper_ipc PRIVATE 
  ${TRUSTY_ROOT}/trusty/user/app/gatekeeper
  ${TRUSTY_ROOT}/trusty/user/app/gatekeeper/ipc)

sea_link_libraries(gatekeeper_ipc gatekeeper_ipc_lib.ir)
sea_overlink_libraries(gatekeeper_ipc trusty_api_override.ir)
sea_overlink_libraries(gatekeeper_ipc realloc_override.ir)
sea_overlink_libraries(gatekeeper_ipc fprintf_override.ir)
sea_overlink_libraries(gatekeeper_ipc time_override.ir)
sea_attach_bc_cc_link(gatekeeper_ipc)
sea_add_unsat_test(gatekeeper_ipc)
