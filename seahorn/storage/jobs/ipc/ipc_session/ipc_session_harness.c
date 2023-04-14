#include <assert.h>
#include <lk/list.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <trusty_log.h>
#include <uapi/err.h>

#include <ipc.h>

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

static void dispatch_event(const uevent_t *ev) {
  sassert(ev);
  if (ev->event == IPC_HANDLE_POLL_NONE) {
    return;
  }

  struct ipc_context *context = ev->cookie;
  // When handle is IPC_INVALID_HANDLE then context
  // is NULL
  if (context) {
    sassert(context->evt_handler);
    sassert(context->handle == ev->handle);

    context->evt_handler(context, ev);
  }
}

static void accept_and_dispatch_request(void) {
  int rc;
  uevent_t event;

  event.handle = INVALID_IPC_HANDLE;
  event.event = 0;
  event.cookie = NULL;
  rc = wait_any(&event, INFINITE_TIME);
  assume(rc == NO_ERROR);

  if (rc == NO_ERROR) { /* got an event */
    dispatch_event(&event);
  }
}

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
  /*
   * The following logic can be used to trigger a CEX (OOB write)
   **/
  /* if (msg_size > 0) { */
  /*   *((char *)msg + msg_size + 1) = 0xFF; */
  /* } */
  uint8_t *snd_buf = malloc(sizeof(msg_size));
  memhavoc(snd_buf, msg_size);
  /* reset modified bit to track unexpected writes */
  sea_reset_modified((char *)snd_buf);
  uint8_t *recv_buf = malloc(sizeof(msg_size));
  memhavoc(recv_buf, msg_size);

  struct iovec tx_iov = {
      .iov_base = snd_buf,
      .iov_len = msg_size,
  };
  struct iovec rx_iov = {
      .iov_base = recv_buf,
      .iov_len = msg_size,
  };
  int rc = sync_ipc_send_msg(context->common.handle, &tx_iov, 1, &rx_iov, 1);

  if (rc < 0)
    return rc;

  if (rc > 0)
    sassert(rc == msg_size);
  // Check that ipc send logic does not modify the message to be sent.
  sassert(!sea_is_modified((char *)snd_buf));
  return NO_ERROR;
}

static struct ipc_channel_context *
sea_channel_onconnect(struct ipc_port_context *parent_ctx,
                      const uuid_t *peer_uuid, handle_t chan_handle) {
  struct ipc_channel_context *pctx = malloc(sizeof(struct ipc_channel_context));
  pctx->ops.on_disconnect = sea_ipc_disconnect_handler;
  pctx->ops.on_handle_msg = sync_ipc_msg_handler;
  // NOTE: Second wait_any call: return channel handle on next wait_any.
  on_waitany_return(chan_handle);
  return pctx;
}

/*
 * Test harness entry point
 *
 * This tests the following
 * 1. This harness establishes a port
 * 2. It then waits for and receives a connection request (port handle)
 * 3. It then creates a channel handle and waits for incoming messages on the
 * channel
 * 4. It receives a message and sends a reply and receives ack.
 *
 * The properties being tested are:
 * 1. spatial memory safety
 * 2. no tampering of messages by ipc layer (messages are never modified after
 * creation)
 *
 * This model uses a rudimentary mock design instead of a fake handle generator.
 * Before every wait_any call, we set the handle to be returned. This is a
 * simpler design than the fake used and is also groundwork for mock-like
 * expectations. For example, checking that a function has been called
 * exactly/atmost/atleast n times. */
int main(void) {
  sea_tracking_on();
// This job is for handle_type_is_ptr model
#ifndef HANDLE_TYPE_IS_PTR
  assert(false);
#endif

  /*  setup port context */
  struct ipc_port_context ctx = {
      .ops = {.on_connect = sea_channel_onconnect},
  };

  /*  call ipc_port_create */
  int rc =
      ipc_port_create(&ctx, STORAGE_DISK_PROXY_PORT, 1, STORAGE_MAX_BUFFER_SIZE,
                      IPC_PORT_ALLOW_TA_CONNECT | IPC_PORT_ALLOW_NS_CONNECT);

  /*  bail out if error but flag it as an error */

  sassert(rc >= 0);

  // NOTE: First wait_any call: return port handle on next wait_any.
  on_waitany_return(ctx.common.handle);

  accept_and_dispatch_request();

  // now expect to handle a channel
  accept_and_dispatch_request();

  /*  check that handle is registered if connection succeeds */
  sassert(sea_ht_is_active_port(ctx.common.handle));

  /*  destroy port */
  ipc_port_destroy(&ctx);
  /*  check that handle is unregistered properly */

  sassert(!sea_ht_is_active_port(ctx.common.handle));
  return 0;
}
