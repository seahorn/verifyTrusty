#include "trusty_ipc.h"
#include <seahorn/seahorn.h>

/* Documentation from trusty API:
  accept()
Accepts an incoming connection and gets a handle to a channel.

long accept(uint32_t handle_id, uuid_t *peer_uuid);
[in] handle_id: Handle representing the port to which a client has connected
[out] peer_uuid: Pointer to a uuud_t structure to be filled with the UUID of
  the connecting client application. It will be set to all zeros if the
  connection originated from the non-secure world.

[retval]: Handle to a channel (if non-negative) on which the server can exchange
  messages with the client (or an error code otherwise)
*/

const char *HANDLE_PATH = "seahorn.com";

bool is_uuid_all_zeros(uuid_t *peer_uuid) {
  return peer_uuid->time_low == 0 && peer_uuid->time_mid == 0 &&
         peer_uuid->time_hi_and_version == 0;
}

int main(void) {

  handle_t port;
  int rc;

  port = port_create(HANDLE_PATH, 1, 100,
                     IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  sassert(!port->secure);

  uuid_t peer_uuid;
  handle_t chan;
  chan = accept(port, &peer_uuid);
  assume(chan != INVALID_IPC_HANDLE);
  sassert(is_uuid_all_zeros(&peer_uuid));

  rc = close(port);
  sassert(rc == 0);

  port = port_create(HANDLE_PATH, 1, 100, IPC_PORT_ALLOW_NS_CONNECT);

  // -- expected secure handle handle
  sassert(port->secure);

  chan = accept(port, &peer_uuid);
  assume(chan != INVALID_IPC_HANDLE);
  return 0;
}
