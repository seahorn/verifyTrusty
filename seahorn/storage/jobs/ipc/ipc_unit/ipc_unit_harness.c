#include <assert.h>
#include <lk/list.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <trusty_log.h>
#include <uapi/err.h>

#include <ipc_mod.h>

// #include "handle_table.h"
#include "sea_handle_table.h"

#include "seahorn/seahorn.h"
#include <nondet.h>

#include "tipc_limits.h"
#include <interface/storage/storage.h>

#define ND __declspec(noalias)

extern void sea_reset_modified(char *);
extern bool sea_is_modified(char *);
extern void sea_tracking_on(void);
extern void sea_tracking_off(void);
extern ND void memhavoc(void *ptr, size_t size);

extern void sea_printf(const char *format, ...);

// extern int do_handle_msg(struct ipc_channel_context *ctx, const uevent_t
// *ev);

static void sea_ipc_disconnect_handler(struct ipc_channel_context *context) {
  if (context)
    free(context);
}

/*
** Send a ND message. Called when a message is received.
**
*/
static int sync_ipc_msg_handler(struct ipc_channel_context *context, void *msg,
                                size_t msg_size) {
  sassert(!sea_is_modified((char *)msg));
  return nd_int();
}

static struct ipc_channel_context *
sea_channel_onconnect(struct ipc_port_context *parent_ctx,
                      const uuid_t *peer_uuid, handle_t chan_handle) {
  struct ipc_channel_context *pctx = malloc(sizeof(struct ipc_channel_context));
  pctx->ops.on_disconnect = sea_ipc_disconnect_handler;
  pctx->ops.on_handle_msg = sync_ipc_msg_handler;
  // NOTE: Second wait_any call: return channel handle on next wait_any.
  return pctx;
}

extern bool check_sequence(void);

int main(void) {
  sea_tracking_on();
// This job is for handle_type_is_ptr model
#ifndef HANDLE_TYPE_IS_PTR
  assert(false);
#endif

  // TODO: replace malloc with alloca
  struct ipc_channel_context *chan_ctx =
      malloc(sizeof(struct ipc_channel_context));
  chan_ctx->ops.on_disconnect = sea_ipc_disconnect_handler;
  chan_ctx->ops.on_handle_msg = sync_ipc_msg_handler;

  uevent_t event;
  maybe_grow_msg_buf(MSG_BUF_MAX_SIZE);
  do_handle_msg(chan_ctx, &event);
}
