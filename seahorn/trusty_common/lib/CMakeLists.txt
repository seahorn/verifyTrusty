# proof helpers
add_library(sea_common_proofs
  handle_table.c
  nondet.c
  bcmp.c
)

if(SEA_ALLOCATOR_CAN_FAIL)
  target_compile_definitions(sea_common_proofs PRIVATE SEA_ALLOCATOR_CAN_FAIL)
endif()
sea_attach_bc(sea_common_proofs)


# crab proof helpers
if(SEA_ENABLE_CRAB)
  add_library(crab_proofs nd_clam.c)
  sea_attach_bc(crab_proofs)
endif()