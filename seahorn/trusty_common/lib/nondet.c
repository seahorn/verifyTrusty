#include "nondet.h"
#include "seahorn/seahorn.h"

int nd_trusty_errs(void) {
  int ret = nd_int();
  assume(ret <= NO_ERROR);
  return ret;
}

int nd_get_msg_ret(void) { return nd_trusty_errs(); }

int nd_put_msg_ret(void) { return nd_trusty_errs(); }

int nd_wait_any_ret(void) { return nd_trusty_errs(); }

uint32_t nd_event_flag(void) {
  uint32_t flag = nd_unsigned();
  assume(flag < (uint32_t)0x16); // max is (1111)2
  return flag;
}

int nd_set_cookie_ret(void) { return nd_trusty_errs(); }

int nd_close_ret(void) { return nd_trusty_errs(); }

int nd_wait_ret(void) { return nd_trusty_errs(); }
