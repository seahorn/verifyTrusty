set(EXTERNAL ${PROJECT_SOURCE_DIR}/trusty/trusty/user/base/lib/libc-trusty)
add_library(sea_libc_trusty
  ${EXTERNAL}/ipc.c
  )
target_include_directories(sea_libc_trusty PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

sea_attach_bc(sea_libc_trusty)

