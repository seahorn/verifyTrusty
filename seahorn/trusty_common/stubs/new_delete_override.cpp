
#include <cstdlib>
#include <new> 
using namespace std;

void *operator new(size_t sz) noexcept {
  if (sz == 0)
    return nullptr;
  if (void *ptr = std::malloc(sz))
    return ptr;
  return nullptr;
}

void *operator new[](size_t sz) noexcept { return ::operator new(sz); }

void *operator new(size_t sz, const std::nothrow_t&) noexcept {
  return ::operator new(sz);
}

void *operator new[](size_t sz, const std::nothrow_t&) noexcept {
  return ::operator new(sz);
}

void operator delete(void *ptr) noexcept { std::free(ptr); }

void operator delete[](void *ptr) noexcept { ::operator delete(ptr); }

void operator delete(void *ptr, const std::nothrow_t&) noexcept { ::operator delete(ptr); }

void operator delete[](void *ptr, const std::nothrow_t&) noexcept { ::operator delete(ptr); }