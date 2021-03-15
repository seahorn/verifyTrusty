add_library(realloc_override realloc_override.c)
sea_attach_bc(realloc_override)

add_library(realloc realloc.c)
sea_attach_bc(realloc)

add_library(fprintf_override fprintf_override.c)
sea_attach_bc(fprintf_override)

add_library(ipc_loop_override ipc_loop_override.c)
target_include_directories(ipc_loop_override 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage)
sea_attach_bc(ipc_loop_override)


add_library(logger logger.cpp)
sea_attach_bc(logger)

add_library(time_override time_override.c)
sea_attach_bc(time_override)

add_library(gatekeeper_ipc_lib
  ${TRUSTY_ROOT}/trusty/user/app/gatekeeper/ipc/gatekeeper_ipc.cpp)
target_compile_definitions(gatekeeper_ipc_lib PRIVATE main=_main)
target_include_directories(gatekeeper_ipc_lib PRIVATE
  ${TRUSTY_ROOT}/trusty/user/app/gatekeeper
  ${TRUSTY_ROOT}/trusty/user/app/gatekeeper/ipc)
sea_attach_bc_cc(gatekeeper_ipc_lib)

add_library(
  trusty_api_override
  trusty_messaging_api_override.c
  trusty_handle_api_override.c
  trusty_server_api_override.c
)
sea_attach_bc(trusty_api_override)

# Externalize static functions
set(LLVMIR_OPT ${SEA_PP})
llvmir_attach_opt_pass_target(TARGET gatekeeper_ipc_lib.opt.ir DEPENDS gatekeeper_ipc_lib.ir
  "--externalize-function=^s_*" "-externalize-functions-delete=false" "--externalize-fns")
