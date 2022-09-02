#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <seahorn/seahorn.h>
#include "nondet.h"

#define NONDET_FN_ATTR __declspec(noalias)

extern NONDET_FN_ATTR int nd_int(void);
extern NONDET_FN_ATTR int64_t nd_int64_t(void);
extern NONDET_FN_ATTR int8_t  nd_int8_t(void);
extern NONDET_FN_ATTR int16_t nd_int16_t(void);
extern NONDET_FN_ATTR int32_t nd_int32_t(void);
extern handle_t NONDET_FN_ATTR nd_handle(void);

extern uint32_t NONDET_FN_ATTR nd_time_low(void);
extern uint16_t NONDET_FN_ATTR nd_time_mid(void);
extern uint16_t NONDET_FN_ATTR nd_time_hi_n_ver(void);

extern int NONDET_FN_ATTR nd_trusty_ipc_err(void);
extern uint32_t nd_trusty_ipc_event(void);

bool nd_malloc_is_fail(void) {
  // make assumption for crab
  // malloc always safe
  return false;
}

size_t nd_size_t(void) {
  int64_t res = nd_int64_t();
  __VERIFIER_assume(res >= 0);
  return (size_t)res;
}  

uint8_t nd_uint8_t(void) {
  int8_t res = nd_int8_t();
  __VERIFIER_assume(res >= 0);
  return (uint8_t)res;  
}

uint16_t nd_uint16_t(void) {
  int16_t res = nd_int16_t();
  __VERIFIER_assume(res >= 0);
  return (uint16_t)res;  
}

uint32_t nd_uint32_t(void) {
  int32_t res = nd_int32_t();
  __VERIFIER_assume(res >= 0);
  return (uint32_t)res;  
}

uint64_t nd_uint64_t(void) {
  int64_t res = nd_int64_t();
  __VERIFIER_assume(res >= 0);
  return (uint64_t)res;    
}

handle_t nd_handle(void) {
  return nd_int32_t();
}

uint32_t nd_time_low(void) {
  return nd_uint32_t();
}

uint16_t nd_time_mid(void) {
  return nd_uint16_t();
}

uint16_t nd_time_hi_n_ver(void) {
  return nd_uint16_t();
}

int nd_trusty_ipc_err(void) {
  return nd_int();
}

uint32_t nd_trusty_ipc_event(void) {
  uint32_t res = nd_uint32_t();
  __VERIFIER_assume(res >= 0 && res <= IPC_HANDLE_POLL_SEND_UNBLOCKED);
  return res;
}

int nd_store_mem_size(void) {
  return nd_int();
}

size_t nd_msg_len(void) {
  return nd_size_t();
}

uint32_t nd_msg_id(void) {
  return nd_uint32_t();
}

uint16_t nd_short(void) {
  return nd_uint16_t();
}

ssize_t nd_ssize_t(void) {
  int64_t res = nd_int64_t();
  return (ssize_t)res;
}

unsigned int nd_uint(void) {
  return nd_uint32_t();
}