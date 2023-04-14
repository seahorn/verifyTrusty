/**
Implementation of table of handles to be used by IPC
*/

/** Global variables that keep current handle information */

#include "sea_handle_table.h"
#include "trusty_ipc.h"
#include <malloc.h>
#include <seahorn/seahorn.h>

#include <stdbool.h>
extern void sea_printf(const char *format, ...);
static unsigned int num_choose_handle = 0;
static unsigned int num_on_waitany_return = 0;

#define ND __declspec(noalias)

// Records the handle to return on wait_any
static handle_t g_on_waitany_handle;
// Records the handle to return on connect
static handle_t g_on_connect_handle;

extern ND bool nd_bool(void);

void *malloc_can_fail(size_t sz) { return nd_bool() ? malloc(sz) : NULL; }

void *malloc_no_fail(size_t sz) { return malloc(sz); }

handle_t ptoh(HandleState *p) {
  // This narrowing operation is safe since ptr is assumed to be aligned.
  return p;
}

HandleState *htop(handle_t h) {
  // This widening operation is safe since ptr is assumed to be aligned.
  return ((HandleState *)(h));
}

bool sea_ht_has_msg(handle_t chan_handle) {
  return htop(chan_handle)->msg.id > INVALID_IPC_MSG_ID;
}

uint32_t sea_ht_get_msg_id(handle_t chan_handle) {
  return htop(chan_handle)->msg.id;
}

// assume channel handle
void sea_ht_set_msg_id(handle_t chan_handle, uint32_t id) {
  htop(chan_handle)->msg.id = id;
}

// assume channel handle
size_t sea_ht_get_msg_len(handle_t chan_handle) {
  return htop(chan_handle)->msg.len;
}

void sea_ht_set_msg_len(handle_t chan_handle, size_t len) {
  htop(chan_handle)->msg.len = len;
}

extern ND size_t nd_msg_len(void);
extern ND size_t nd_size(void);
extern ND uint32_t nd_msg_id(void);

void sea_ht_new_nd_msg(handle_t chan_handle) {
  htop(chan_handle)->msg.len = nd_msg_len();
  htop(chan_handle)->msg.id = nd_msg_id();
  assume(htop(chan_handle)->msg.id > INVALID_IPC_MSG_ID);
  return;
}

/**
 * Set handle to inactive
 */
void sea_ht_free(handle_t handle) {
  // we cannot "free" the handle since we might want to query state after this
  // call
  handle->active = false;
}

/**
Allocates a new port handle

Return INVALID_IPC_HANDLE if no handle is available to be allocated
*/
handle_t sea_ht_new_port(bool secure, const char *path) {
#if CAN_RETURN_INVALID_IPC_HANDLE
  HandleState *h = (HandleState *)malloc_can_fail(sizeof(HandleState));
#else
  HandleState *h = (HandleState *)malloc_no_fail(sizeof(HandleState));
#endif
  if (!h)
    return INVALID_IPC_HANDLE;
  h->active = true;
  h->secure = secure;
  h->type = port;
  h->cookie = NULL;
  h->path = path;
  sea_ht_new_nd_msg(h);
  return ptoh(h);
}

// TODO: should be "match"
handle_t sea_ht_math_port(const char *path) {
  if (g_on_connect_handle == NULL) {
    return INVALID_IPC_HANDLE;
  }
  // Check that the user followed the convention that identical paths
  // have the same address.
  sassert(g_on_connect_handle->path == path);
  return g_on_connect_handle;
}

/**
Non-deterministically chooses an active handle

Blocks if no active handle is available
*/
// TODO: Change spec to include invalid handle
handle_t sea_ht_choose_active_handle(void) {
  if (g_on_waitany_handle == NULL) {
    return INVALID_IPC_HANDLE;
  }
  // Check that the user set an active handle
  sassert(g_on_waitany_handle->active == true);
  handle_t handle = g_on_waitany_handle;
  // NOTE: unset the handle after one use
  g_on_waitany_handle = NULL;
  return handle;
}

handle_t sea_ht_new_channel(handle_t parent_port) {
#if CAN_RETURN_INVALID_IPC_HANDLE
  HandleState *c = (HandleState *)malloc_can_fail(sizeof(HandleState));
#else
  HandleState *c = (HandleState *)malloc_no_fail(sizeof(HandleState));
#endif
  if (!c)
    return INVALID_IPC_HANDLE;
  c->active = true;
  c->type = channel;
  c->cookie = NULL;
  c->path = NULL;
  sea_ht_new_nd_msg(c);
  return ptoh(c);
}

// return true if passed handle is port type and active
bool sea_ht_is_active_port(handle_t handle) {
  return htop(handle)->type == port && htop(handle)->active == true;
}

// assume we are passed a port
void sea_ht_set_cookie_port(handle_t handle, void *cookie) {
  htop(handle)->cookie = cookie;
}

// assume we are passed a port
void *sea_ht_get_cookie_port(handle_t handle) { return htop(handle)->cookie; }

// assume we are passed a channel
void sea_ht_set_cookie_channel(handle_t handle, void *cookie) {
  htop(handle)->cookie = cookie;
}

// assume we are passed a channel
void *sea_ht_get_cookie_channel(handle_t handle) {
  return handle == NULL ? NULL : htop(handle)->cookie;
}

// set cookie for both port/channel type
void sea_ht_set_cookie(handle_t handle, void *cookie) {
  htop(handle)->cookie = cookie;
}

// get cookie for both port/channel type
void *sea_ht_get_cookie(handle_t handle) {
  return handle == NULL ? NULL : htop(handle)->cookie;
}

// Set the handle to return on next wait_any call.
// This remains set unless unset explicitly.
void on_waitany_return(handle_t handle) { g_on_waitany_handle = handle; }

// Set the handle to return on next connect call
// This remains set unless unset explicitly.
void on_connect_return(handle_t handle) { g_on_connect_handle = handle; }
