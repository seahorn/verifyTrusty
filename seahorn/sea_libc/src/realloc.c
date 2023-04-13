#include <sea_allocators.h>

void *realloc(void *ptr, size_t new_size) { return sea_realloc(ptr, new_size); }
