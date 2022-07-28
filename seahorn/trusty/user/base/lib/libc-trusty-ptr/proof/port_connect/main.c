#include "trusty_ipc.h"
#include "uapi/err.h"
#include <sea_handle_table.h>
#include <seahorn/seahorn.h>

/* Documentation from trusty API:
  connect()
Initiates a connection to a port specified by name.

long connect(const char *path, uint flags);

[in] path: Name of a port published by a Trusty application
[in] flags: Specifies additional, optional behavior

[retval]: Handle to a channel over which messages can be exchanged
with the server; error if negative
*/

const char *PATH = "ta.seahorn.com";

int main(void) {

  handle_t h1 = port_create(PATH, 1, 100, IPC_PORT_ALLOW_TA_CONNECT);

  // expect non-secure handle
  sassert(!h1->secure);

  handle_t h2 = port_create(PATH, 1, 100, IPC_PORT_ALLOW_NS_CONNECT);

  // expect secure handle
  sassert(h2->secure);

  handle_t rc;

  // connect will connect to h2
  on_connect_return(h2);

  rc = connect(PATH, IPC_CONNECT_ASYNC | IPC_CONNECT_WAIT_FOR_PORT);
  // expect valid connection
  sassert(rc > 0);

  rc = connect(PATH, IPC_CONNECT_ASYNC | IPC_CONNECT_WAIT_FOR_PORT);
  // expect valid connection
  sassert(rc > 0);

  return 0;
}
