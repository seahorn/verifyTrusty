#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <trusty_ipc.h> // -> ipc structs

#include <uapi/err.h> // trusty errors definitions
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
// generic
long nd_long(void);
uint16_t nd_short(void);
int32_t nd_int(void);
bool nd_bool(void);
uint8_t nd_char(void);
uint32_t nd_unsigned(void);
uint32_t nd_uint32_t(void);
uint64_t nd_uint64_t(void);
void* nd_ptr(void);
size_t nd_size_t(void);
ssize_t nd_ssize_t(void);
unsigned int nd_uint(void);

/**
 * for functions with return pattern:
 * NO_ERROR on success; a negative error otherwise
 **/
int nd_trusty_errs(void);
handle_t nd_handle(void);

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

/* ipc */
uint32_t nd_trusty_ipc_event(void);
int nd_trusty_ipc_err(void);

/* */
void memhavoc(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif
