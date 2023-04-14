option(SEA_ALLOCATOR_CAN_FAIL "Use can fail allocator" OFF)
add_library(sea_libc
  src/realloc.c
  src/sea_allocators.c)
target_include_directories(sea_libc PUBLIC include)
sea_attach_bc(sea_libc)
