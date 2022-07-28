set(EXTERNAL ${TRUSTY_ROOT}/trusty/user/base/lib/libc-trusty)
add_library(
  sea_libc_trusty
  ${EXTERNAL}/ipc.c
)

if (HANDLE_TYPE_IS_PTR)
  add_subdirectory(libc-trusty-ptr)
else ()
  add_subdirectory(libc-trusty)
endif ()
add_compile_definitions(CAN_RETURN_INVALID_IPC_HANDLE=0)
