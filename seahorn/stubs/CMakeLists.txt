add_library(realloc_override realloc_override.c)
sea_attach_bc(realloc_override)

add_library(realloc realloc.c)
sea_attach_bc(realloc)

add_library(trusty_msg trusty_msg.c)
sea_attach_bc(trusty_msg)

add_library(fprintf_override fprintf_override.c)
sea_attach_bc(fprintf_override)

add_library(logger logger.cpp)
sea_attach_bc(logger)

add_library(wide_trusty_msg wide/trusty_msg.c)
sea_attach_bc(wide_trusty_msg)

add_library(time_override time_override.c)
sea_attach_bc(time_override)

add_library(gatekeeper_ipc_lib
  ${TRUSTY_ROOT}/trusty/user/app/gatekeeper/ipc/gatekeeper_ipc.cpp)
target_compile_definitions(gatekeeper_ipc_lib PRIVATE main=_main)
sea_attach_bc_cc(gatekeeper_ipc_lib)

# Externalize static functions
set(LLVMIR_OPT ${SEA_PP})
llvmir_attach_opt_pass_target(TARGET gatekeeper_ipc_lib.opt.ir DEPENDS gatekeeper_ipc_lib.ir
  "--externalize-function=^s_*" "-externalize-functions-delete=false" "--externalize-fns")
