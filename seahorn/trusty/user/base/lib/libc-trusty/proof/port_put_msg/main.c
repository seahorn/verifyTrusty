#include <seahorn/seahorn.h>
#include "trusty_ipc.h"
#include "uapi/err.h"
/* Documentation from trusty API:
  put_msg()
Retires a message with a specified ID.

long put_msg(uint32_t handle, uint32_t msg_id);
[in] handle: Handle of the channel on which the message has arrived.
[in] msg_id: ID of message being retired.

[retval]: NO_ERROR on success; a negative error otherwise.
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

          // discard msg
          rc = put_msg(chan, msg_info.id);
          assume(rc == NO_ERROR);
          rc = read_msg(chan, msg_info.id, 0, &msg);
          sassert(rc < NO_ERROR);
        }
      }
    }
  }

  

  return 0;
}
