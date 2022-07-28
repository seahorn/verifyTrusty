#include <seahorn/seahorn.h>
#include "trusty_ipc.h"

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

bool is_uuid_all_zeros(uuid_t* peer_uuid) {
  return peer_uuid->time_low == 0 && peer_uuid->time_mid == 0
   && peer_uuid->time_hi_and_version == 0;
}

int main(void) {

  handle_t port;
  int rc;

  port =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  sassert(port == 2);

  uuid_t peer_uuid;
  handle_t chan;
  chan = accept(port, &peer_uuid);
  assume(chan != INVALID_IPC_HANDLE);
  sassert(chan == 16);
  sassert(is_uuid_all_zeros(&peer_uuid));

  rc = close(port);
  sassert(rc == 0);

  port =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT);

  // -- expected secure handle handle
  sassert(port == 1);
  
  chan = accept(port, &peer_uuid);
  assume(chan != INVALID_IPC_HANDLE);
  sassert(chan == 17);

  return 0;
}
