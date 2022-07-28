#include <sea_ipc_helper.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <seahorn/seahorn.h>
#include <uapi/err.h>
#include <ipc.h>

void sea_dispatch_event(const uevent_t* ev) {
    sassert(ev);

    if (ev->event == IPC_HANDLE_POLL_NONE) {
        return;
    }

    struct ipc_context* context = ev->cookie;
    sassert(context);
    sassert(context->evt_handler);
    sassert(context->handle == ev->handle);

    context->evt_handler(context, ev);
}

static void sea_ipc_disconnect_handler(struct ipc_channel_context *context) {
  if (context)
    free(context);
}

static int sea_ipc_msg_handler(struct ipc_channel_context *context, void *msg,
                               size_t msg_size) {
  sassert(msg_size <= MSG_BUF_MAX_SIZE);
  struct iovec iov = {
      .iov_base = msg,
      .iov_len = msg_size,
  };
  ipc_msg_t i_msg = {
      .iov = &iov,
      .num_iov = 1,
  };
  int rc = send_msg(context->common.handle, &i_msg);
  if (rc < 0) {
    return rc;
  }
  return NO_ERROR;
}

static int sync_ipc_msg_handler(struct ipc_channel_context *context, void *msg,
                               size_t msg_size) {
  uint8_t *snd_buf = malloc(sizeof(msg_size));
  uint8_t *recv_buf = malloc(sizeof(msg_size));
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

  return NO_ERROR;
}

/*
 * directly return a channel context given uuid and chan handle
 */
static struct ipc_channel_context *
sea_channel_connect(struct ipc_port_context *parent_ctx,
                    const uuid_t *peer_uuid, handle_t chan_handle) {
  struct ipc_channel_context *pctx = malloc(sizeof(struct ipc_channel_context));
  pctx->ops.on_disconnect = sea_ipc_disconnect_handler;
  pctx->ops.on_handle_msg = sea_ipc_msg_handler;
  return pctx;
}

/*
 * directly return a channel context given uuid and chan handle
 */
struct ipc_channel_context *
sea_sync_channel_connect(struct ipc_port_context *parent_ctx,
                    const uuid_t *peer_uuid, handle_t chan_handle) {
  struct ipc_channel_context *pctx = malloc(sizeof(struct ipc_channel_context));
  pctx->ops.on_disconnect = sea_ipc_disconnect_handler;
  pctx->ops.on_handle_msg = sync_ipc_msg_handler;
  return pctx;
}

/*
 * constant variable of ipc_port_context
 */
static struct ipc_port_context port_ctx = {
      .ops = {.on_connect = sea_channel_connect},
  };

struct ipc_port_context* create_port_context(){
  return &port_ctx;
}