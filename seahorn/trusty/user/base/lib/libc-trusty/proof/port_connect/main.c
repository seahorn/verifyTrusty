#include <seahorn/seahorn.h>
#include <trusty_ipc.h>
#include <uapi/err.h>
/* Documentation from trusty API:
  connect()
Initiates a connection to a port specified by name.

long connect(const char *path, uint flags);

[in] path: Name of a port published by a Trusty application
[in] flags: Specifies additional, optional behavior

[retval]: Handle to a channel over which messages can be exchanged 
with the server; error if negative
*/

int main(void) {

  handle_t h1 =
      port_create("ta.seahorn.com", 1, 100, IPC_PORT_ALLOW_TA_CONNECT);

  // expect non-secure handle
  sassert(h1 == 2);

  handle_t h2 =
      port_create("ns.seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT);

  // expect secure handle
  sassert(h2 == 1);

  handle_t rc;

  rc = connect("ta.seahorn.com", IPC_CONNECT_ASYNC | IPC_CONNECT_WAIT_FOR_PORT);
  // expect valid connection
  sassert(rc > 0);

  rc = connect("ns.seahorn.com", IPC_CONNECT_ASYNC | IPC_CONNECT_WAIT_FOR_PORT);
  // expect valid connection
  sassert(rc > 0);

  return 0;
}
