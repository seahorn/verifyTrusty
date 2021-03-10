# proof helpers
add_library(sea_proofs
  handle_table.c
  sea_ipc_helper.c
)

target_include_directories(sea_proofs PRIVATE
  ${TRUSTY_ROOT}/trusty/user/app/storage) 


if(SEA_ALLOCATOR_CAN_FAIL)
  target_compile_definitions(sea_proofs PRIVATE SEA_ALLOCATOR_CAN_FAIL)
endif()
sea_attach_bc(sea_proofs)
