# proof helpers
add_library(
  sea_proofs
  handle_table.c
)
if(SEA_ALLOCATOR_CAN_FAIL)
  target_compile_definitions(sea_proofs PRIVATE SEA_ALLOCATOR_CAN_FAIL)
endif()
sea_attach_bc(sea_proofs)