#include <stdint.h>
#include <stdlib.h>
#include <trusty_ipc.h> // -> ipc structs

#include <uapi/err.h> // trusty errors definitions
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Mark nondet functions as not accessing memory
 * Note: if the attribute is too agressive, the optimizer might remove
 *       calls to nondet functions, thus reducing the amount of non-determinism.
 * Note: using each nondet function only once grately simplifies debugging.
 */
#define NONDET_FN_ATTR __declspec(noalias)

// generic
long nd_long(void);
uint16_t nd_short(void);
int32_t nd_int(void);
char nd_char(void);
uint32_t nd_unsigned(void);
void *nd_ptr(void);

/**
 * for functions with return pattern:
 * NO_ERROR on success; a negative error otherwise
 **/
int nd_trusty_errs(void);

/* port_create */
handle_t nd_port_handle(void);

/* get_msg */
size_t nd_msg_len(void);
uint32_t nd_msg_id(void);
int nd_get_msg_ret(void);

/* read_msg */
ssize_t nd_read_msg_ret(void);
uint8_t nd_msg_element(void);

/* send_msg */
ssize_t nd_send_msg_ret(void);

/* put_msg */
int nd_put_msg_ret(void);

/* wait_any */
handle_t nd_wait_handle(void);
int nd_wait_any_ret(void);
uint32_t nd_event_flag(void);

/* set_cookie */
int nd_set_cookie_ret(void);

/* accept */
handle_t nd_chan_handle(void);
uint32_t nd_time_low(void);
uint16_t nd_time_mid(void);
uint16_t nd_time_hi_n_ver(void);

/* close */
int nd_close_ret(void);

/* wait */
int nd_wait_ret(void);

/* store allocated mem size */
int nd_store_mem_size(void);
size_t nd_get_alloc_size(void);

#ifdef __cplusplus
}
#endif
