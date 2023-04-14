set(TRUSTY_IPC_DIR ${TRUSTY_ROOT}/trusty/user/base/lib/libc-trusty)

add_library(
  sea_libc_trusty
  ${TRUSTY_IPC_DIR}/ipc.c
)
if (HANDLE_TYPE_IS_PTR)
  add_subdirectory(libc-trusty-ptr)
else ()
  add_subdirectory(libc-trusty)
endif ()
add_compile_definitions(CAN_RETURN_INVALID_IPC_HANDLE=0)
