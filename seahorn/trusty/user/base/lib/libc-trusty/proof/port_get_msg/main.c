#include <seahorn/seahorn.h>
#include "trusty_ipc.h"
#include "uapi/err.h"
/* Documentation from trusty API:
  get_msg()
Gets meta-information about the next message in an incoming message queue
of a specified channel.

long get_msg(uint32_t handle, ipc_msg_info_t *msg_info);
[in] handle: Handle of the channel on which a new message must be retrieved.
[out] msg_info: Message information structure described as follows:
typedef struct ipc_msg_info {
        size_t    len;
        uint32_t  id;
} ipc_msg_info_t;
Each message is assigned a unique ID across the set of outstanding messages,
and the total length of each message is filled in.
If configured and allowed by the protocol,
there can be multiple outstanding (opened) messages at once for a particular
channel.

[retval]: NO_ERROR on success; a negative error otherwise
*/

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
      /* incoming connection: accept it */
      handle_t chan = accept(port, &peer_uuid);
      assume(chan != INVALID_IPC_HANDLE);
      sassert(chan == 16);

      uevent_t cev;

      rc = wait(chan, &cev, INFINITE_TIME);
      sassert(rc <= NO_ERROR);
      if (rc == NO_ERROR) {
        if (cev.event & IPC_HANDLE_POLL_MSG) {
          ipc_msg_info_t msg_info;

          /* get message info */
          rc = get_msg(chan, &msg_info);
          if (rc == NO_ERROR) {
            sassert(msg_info.id != 0);
            sassert(msg_info.len >= 0);
          }

          rc = close(chan);
          sassert(rc == NO_ERROR);
        }
      }
    }
  }

  return 0;
}
