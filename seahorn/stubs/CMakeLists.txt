add_library(realloc_override realloc_override.c)
sea_attach_bc(realloc_override)

add_library(realloc realloc.c)
sea_attach_bc(realloc)

add_library(trusty_msg trusty_msg.c)
sea_attach_bc(trusty_msg)

add_library(fprintf_override fprintf_override.c)
sea_attach_bc(fprintf_override)

add_library(logger logger.cpp)
sea_attach_bc(logger)

add_library(wide_trusty_msg wide/trusty_msg.c)
sea_attach_bc(wide_trusty_msg)

add_library(time_override time_override.c)
sea_attach_bc(time_override)
