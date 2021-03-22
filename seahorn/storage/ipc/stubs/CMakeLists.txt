add_library(ipc_loop_override ipc_loop_override.c)
target_include_directories(ipc_loop_override 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage)
sea_attach_bc(ipc_loop_override)