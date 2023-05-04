add_library(ipc_loop_override ipc_loop_override.c)
target_include_directories(ipc_loop_override 
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage)
sea_attach_bc(ipc_loop_override)

add_library(ipc_syscalls_override syscalls.c)
target_include_directories(ipc_syscalls_override
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage)
sea_attach_bc(ipc_syscalls_override)

add_library(ipc_syscalls_mock_override syscalls_mock.cc)
target_include_directories(ipc_syscalls_mock_override
  PRIVATE ${TRUSTY_ROOT}/trusty/user/app/storage ${CMAKE_CURRENT_SOURCE_DIR})
sea_attach_bc_cc(ipc_syscalls_mock_override)
