#include "trusty_ipc.h"
#include <seahorn/seahorn.h>

const char *PATH = "seahorn.com";

int main(void) {

  handle_t port;

  port = port_create(PATH, 1, 100,
                     IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  sassert(!port->secure);

  int rc;

  // release handle
  rc = close(port);
  sassert(rc == 0);
  sassert(!port->active);

  // check if a new port can be created after close
  port = port_create(PATH, 1, 100,
                     IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  sassert(!port->secure);

  return 0;
}
