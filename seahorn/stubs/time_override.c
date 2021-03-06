#include "nondet.h"
#include <time.h>

int trusty_gettime(clockid_t clock_id, int64_t* time) {
    return nd_int();
}
