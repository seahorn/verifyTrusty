#include <lk/compiler.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/uio.h>
#include <uapi/err.h> // trusty errors definitions
#include <uapi/trusty_uuid.h>

#include "seahorn/seahorn.h"
#include <nondet.h>

#define ND __declspec(noalias)
#define INVALID_IPC_MSG_ID 0
extern ND int nd_trusty_ipc_err(void);
extern ND size_t nd_msg_len(void);
extern ND size_t nd_size(void);
extern ND uint32_t nd_msg_id(void);
extern ND void memhavoc(void *ptr, size_t size);

int wait_any(uevent_t *event, uint32_t timeout_msecs);
int get_msg(handle_t handle, ipc_msg_info_t *msg_info);
ssize_t read_msg(handle_t handle, uint32_t msg_id, uint32_t offset,
                 ipc_msg_t *msg);
int put_msg(handle_t handle, uint32_t msg_id);
ssize_t send_msg(handle_t handle, ipc_msg_t *msg);

static int msg_size;

int get_msg(handle_t handle, ipc_msg_info_t *msg_info) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;

  msg_info->id = nd_msg_id();
  msg_info->len = nd_msg_len();
  msg_size = msg_info->len;
  return msg_info->id != INVALID_IPC_MSG_ID ? NO_ERROR : ERR_GENERIC;
}

ssize_t read_msg(handle_t handle, uint32_t msg_id, uint32_t offset,
                 ipc_msg_t *msg) {
  // assumptions for simple model
  // preconditions
  sassert(msg->num_iov == 1);
  // *((char *)msg + msg_size + 1) = 0xFF;
  memhavoc(msg, msg_size);
  return msg_size;
}

int put_msg(handle_t handle, uint32_t msg_id) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;
  // TODO: set msg id to invalid
  // sea_ht_set_msg_id(handle, INVALID_IPC_MSG_ID);
  return NO_ERROR;
}
