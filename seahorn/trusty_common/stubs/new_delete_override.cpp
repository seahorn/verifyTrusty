
#include <cstdlib>
#include <new>
using namespace std;

/**
 * We override the impl of new operator. However, we seem to diverge from c++
 * spec. We implement the version that expects to throw an error if alloc fails
 * (and never return nullptr) but we forbid throwing and instead return nullptr.
 * This warning is now an error with llvm14 and we fail compilation.
 **/
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
// void *operator new(size_t sz) noexcept {
//   if (sz == 0)
//     return nullptr;
//   if (void *ptr = std::malloc(sz))
//     return ptr;
//   return nullptr;
// }
// #pragma clang diagnostic pop

// void *operator new[](size_t sz) noexcept { return ::operator new(sz); }

// void *operator new(size_t sz, const std::nothrow_t &) noexcept {
//   return ::operator new(sz);
// }

// void *operator new[](size_t sz, const std::nothrow_t &) noexcept {
//   return ::operator new(sz);
// }

// void operator delete(void *ptr) noexcept { std::free(ptr); }

// void operator delete[](void *ptr) noexcept { ::operator delete(ptr); }

// void operator delete(void *ptr, const std::nothrow_t &) noexcept {
//   ::operator delete(ptr);
// }

// void operator delete[](void *ptr, const std::nothrow_t &) noexcept {
//   ::operator delete(ptr);
// }
