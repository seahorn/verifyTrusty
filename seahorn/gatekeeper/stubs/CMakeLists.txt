add_library(gatekeeper_ipc_lib
  ${TRUSTY_ROOT}/trusty/user/app/gatekeeper/ipc/gatekeeper_ipc.cpp)
target_compile_definitions(gatekeeper_ipc_lib PRIVATE main=_main)
target_include_directories(gatekeeper_ipc_lib PRIVATE
  ${TRUSTY_ROOT}/trusty/user/app/gatekeeper
  ${TRUSTY_ROOT}/trusty/user/app/gatekeeper/ipc
  ${TRUSTY_ROOT}/system/gatekeeper/include/gatekeeper)
sea_attach_bc_cc(gatekeeper_ipc_lib)

# Externalize static functions
# C++ static function transfer to name with _Z<name>v
set(LLVMIR_OPT ${SEA_PP})
llvmir_attach_opt_pass_target(TARGET gatekeeper_ipc_lib.opt.ir DEPENDS gatekeeper_ipc_lib.ir
  "--externalize-function=^_Z*" "-externalize-functions-delete=false" "--externalize-fns")