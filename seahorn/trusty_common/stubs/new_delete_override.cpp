
#include <cstdlib>
using namespace std;

void *operator new[](size_t sz) noexcept {
    if (sz == 0) return nullptr;
    if (void *ptr = std::malloc(sz))
        return ptr;
    return nullptr;
}

void operator delete(void* ptr) noexcept
{
    std::free(ptr);
}