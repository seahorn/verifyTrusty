#include <seahorn/seahorn.h>
#include <trusty_ipc.h>
#include <uapi/err.h>
/* Documentation from trusty API:
  read_msg()
Reads the content of the message with the specified ID starting from the 
specified offset.

long read_msg(uint32_t handle, uint32_t msg_id, uint32_t offset, ipc_msg_t
*msg);
[in] handle: Handle of the channel from which to read the message.
[in] msg_id: ID of the message to read.
[in] offset: Offset into the message from which to start reading.
[in] msg: Pointer to the ipc_msg_t structure describing a set of buffers into 
which to store incoming message data.

[retval]: Total number of bytes stored in the dst buffers on success; 
a negative error otherwise.
*/

# define MAX_ECHO_MSG_SIZE 64

int main(void) {

  handle_t port =
      port_create("ta.seahorn.com", 1, 100, IPC_PORT_ALLOW_TA_CONNECT);

  // expect non-secure handle
  sassert(port == 2);

  uevent_t event = {.handle = INVALID_IPC_HANDLE, .event = 0, .cookie = NULL};

  int rc = wait(port, &event, INFINITE_TIME);
  sassert(rc <= NO_ERROR);
  if (rc == NO_ERROR) {
    /* got an event */
    sassert(event.handle == port);
    // No cookie set
    sassert(!event.cookie);

    // handle port event
    uuid_t peer_uuid;
    if (event.event & IPC_HANDLE_POLL_READY) {
      handle_t chan = accept(port, &peer_uuid);
      assume(chan != INVALID_IPC_HANDLE);
      sassert(chan == 16);

      uevent_t cev;

      rc = wait(chan, &cev, INFINITE_TIME);
      sassert(rc <= NO_ERROR);
      if (rc == NO_ERROR) {
        if (cev.event & IPC_HANDLE_POLL_MSG) {
          ipc_msg_info_t msg_info;
          rc = get_msg(chan, &msg_info);
          assume(rc == NO_ERROR);
          sassert(msg_info.id != 0);

          uint8_t buf[MAX_ECHO_MSG_SIZE];
          struct iovec iov = {.iov_base = buf, .iov_len = sizeof(buf)};
          ipc_msg_t msg = {.num_iov = 1, .iov = &iov, .num_handles = 1, .handles = NULL};
          rc = read_msg(chan, msg_info.id, 0, &msg);
          assume(rc >= NO_ERROR);
          sassert(rc <= MAX_ECHO_MSG_SIZE);

          /* cleanup */
          rc = put_msg(chan, msg_info.id);
          assume(rc == NO_ERROR);
          rc = close(chan);
          sassert(rc == NO_ERROR);
        }
      }
    }
  }

  

  return 0;
}
