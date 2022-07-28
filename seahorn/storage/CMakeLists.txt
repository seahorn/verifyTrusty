include_directories(include)

add_compile_definitions(CAN_RETURN_INVALID_IPC_HANDLE=1)
if (HANDLE_TYPE_IS_PTR)
  set(IPC_SRC ${TRUSTY_MOD_ROOT}/trusty/user/app/storage/ipc.c)
  add_compile_definitions(HANDLE_TYPE_IS_PTR=1)
else ()
  set(IPC_SRC ${TRUSTY_ROOT}/trusty/user/app/storage/ipc.c)
endif ()

add_subdirectory(lib)
add_subdirectory(stubs)

# The following jobs will be built
add_subdirectory(jobs/ipc/indirect_handlers)
add_subdirectory(jobs/ipc/msg_buffer)
add_subdirectory(jobs/ipc/port_create_destroy)