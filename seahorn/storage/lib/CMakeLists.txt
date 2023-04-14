# storage proof helpers
add_library(sea_storage_ipc_proofs
  sea_ipc_helper.c
)

target_include_directories(sea_storage_ipc_proofs PRIVATE
  ${TRUSTY_ROOT}/trusty/user/app/storage)


if(SEA_ALLOCATOR_CAN_FAIL)
  target_compile_definitions(sea_storage_ipc_proofs PRIVATE SEA_ALLOCATOR_CAN_FAIL)
endif()
sea_attach_bc(sea_storage_ipc_proofs)
