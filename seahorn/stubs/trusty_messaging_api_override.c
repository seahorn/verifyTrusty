/** Verify Trusty

    Implementation of TrustyOS Messaging API
    https://source.android.com/security/trusty/trusty-ref#messaging_api

 */

#include "handle_table.h"
#include "nondet.h"
#include "sea_mem_helper.h"
#include "seahorn/seahorn.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

// trusty reference for definitions only
#include <trusty_ipc.h> // -> ipc structs
#include <uapi/err.h>   // NO_ERROR definition

static uint32_t cur_msg_id;
static bool cur_msg_retired = true;

static bool msg_retired(uint32_t msg_id) {
  return msg_id == cur_msg_id && cur_msg_retired;
}

// get len of first iov of msg if there is any, otherwise 0
static size_t msg_capacity(ipc_msg_t *msg) {
  struct iovec *iovecs = msg->iov;
  if (msg->num_iov == 0)
    return 0;
  return iovecs[0].iov_len;
}

/** messaging **/
/** Gets meta-information about the next message in an incoming message queue */
int get_msg(handle_t handle, ipc_msg_info_t *msg_info) {
  (void)handle;
  int retval = nd_get_msg_ret();
  size_t msg_len = nd_msg_len();
  uint32_t msg_id = nd_msg_id();
  if (retval == NO_ERROR) {
    assume(msg_len > 0);
    assume(msg_id > 0);
    cur_msg_id = msg_id;
    cur_msg_retired = false;
  } else {
    msg_len = 0;
    msg_id = 0;
  }
  msg_info->len = msg_len;
  msg_info->id = msg_id;
  return retval;
}

/*  Reads the content of the message with the specified ID starting from the
 * specified offset. */
ssize_t read_msg(handle_t handle, uint32_t msg_id, uint32_t offset,
                 ipc_msg_t *msg) {
  // return Total number of bytes stored in the dst buffers on success;
  // a negative error otherwise
  (void)handle;
  ssize_t ret = nd_read_msg_ret();
  if (ret >= 0) {
    size_t cap = msg_capacity(msg);
    if (offset >= cap) {
      return ERR_GENERIC;
    }
    // cannot read more than capacity
    assume(ret <= (cap - offset));
  }
  return ret;
}

/* Sends a message over a specified channel.*/
ssize_t send_msg(handle_t handle, ipc_msg_t *msg) {
  // Total number of bytes sent on success; a negative error otherwise
  (void)handle;
  ssize_t ret = nd_send_msg_ret();
  if (ret >= 0) {
    size_t cap = msg_capacity(msg);
    // cannot send more than capacity
    assume(ret <= cap);
  }
  return ret;
}

/* Retires a message with a specified ID */
int put_msg(handle_t handle, uint32_t msg_id) {
  // return NO_ERROR on success; a negative error otherwise
  // assume(retval == NO_ERROR || retval < 0);
  (void)handle;
  if (cur_msg_id == msg_id) {
    cur_msg_retired = true;
  }
  return nd_int();
}
