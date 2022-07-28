#include "trusty_ipc.h"
#include "uapi/err.h"
#include <seahorn/seahorn.h>

int main(void) {

  handle_t port =
      port_create("ta.seahorn.com", 1, 100, IPC_PORT_ALLOW_TA_CONNECT);

  // expect non-secure handle
  sassert(!port->secure);

  uevent_t event = {.handle = INVALID_IPC_HANDLE, .event = 0, .cookie = NULL};

  int rc = wait(port, &event, 0);
  sassert(rc <= NO_ERROR);
  if (rc == NO_ERROR) {
    /* got an event */
    sassert(event.handle == port);
    // No cookie set
    sassert(!event.cookie);
  }

  uuid_t peer_uuid;
  handle_t chan = accept(port, &peer_uuid);
  assume(chan != INVALID_IPC_HANDLE);
  sassert(chan->type == channel);

  rc = wait(chan, &event, 0);
  sassert(rc <= NO_ERROR);
  if (rc == NO_ERROR) {
    if (event.event & IPC_HANDLE_POLL_MSG) {
      struct ipc_msg_info msg_info;
      rc = get_msg(chan, &msg_info);
      assume(rc == NO_ERROR);
      sassert(msg_info.id != 0);

      char buf[4096];
      struct iovec iov = {.iov_base = buf, .iov_len = sizeof(buf)};
      struct ipc_msg msg = {.num_iov = 1, .iov = &iov};
      rc = read_msg(chan, msg_info.id, 0, &msg);
      assume(rc >= NO_ERROR);
      sassert(rc <= 4096);

      rc = put_msg(chan, msg_info.id);
      assume(rc == NO_ERROR);
      rc = read_msg(chan, msg_info.id, 0, &msg);
      sassert(rc < NO_ERROR);
    }
  }

  return 0;
}
