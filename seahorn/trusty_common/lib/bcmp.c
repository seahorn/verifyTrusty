#include <seahorn/seahorn.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>

#define INLINE __attribute__((always_inline))

INLINE void *__memcpy_chk(void *dest, const void *src, size_t len,
                          size_t dstlen) {
  sassert(!(dstlen < len));
  return __builtin_memcpy(dest, src, len);
}

INLINE void *__memset_chk(void *dest, int c, size_t len, size_t dstlen) {
  sassert(!(dstlen < len));
  return __builtin_memset(dest, c, len);
}