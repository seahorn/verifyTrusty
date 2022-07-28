add_library(realloc_override realloc_override.c)
sea_attach_bc(realloc_override)

add_library(realloc realloc.c)
sea_attach_bc(realloc)

add_library(fprintf_override fprintf_override.c)
sea_attach_bc(fprintf_override)

add_library(logger logger.cpp)
sea_attach_bc_cc(logger)

add_library(time_override time_override.c)
sea_attach_bc(time_override)

add_library(new_delete_override new_delete_override.cpp)
sea_attach_bc_cc(new_delete_override)
