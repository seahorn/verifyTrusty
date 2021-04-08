#ifndef _SEA_HANDLE_TABLE_H_
#define _SEA_HANDLE_TABLE_H_

#include <stdbool.h>
#include <trusty_ipc.h>

handle_t sea_ht_new_port(bool secure);
handle_t sea_ht_new_channel(handle_t port);

handle_t sea_ht_choose_active_handle(void);

bool sea_ht_is_active_port(handle_t handle);

void sea_ht_set_cookie_port(handle_t handle, void *cookie);
void *sea_ht_get_cookie_port(handle_t handle);

void sea_ht_set_cookie_channel(handle_t handle, void *cookie);
void *sea_ht_get_cookie_channel(handle_t handle);

void sea_ht_set_cookie(handle_t handle, void *cookie);
void *sea_ht_get_cookie(handle_t handle);

void sea_ht_free(handle_t handle);

bool sea_ht_has_msg(handle_t chan_handle);
uint32_t sea_ht_get_msg_id(handle_t chan_handle);
void sea_ht_set_msg_id(handle_t chan_handle, uint32_t id);
size_t sea_ht_get_msg_len(handle_t chan_handle);
void sea_ht_set_msg_len(handle_t chan_handle, size_t len);
#endif
