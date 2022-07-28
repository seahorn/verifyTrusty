#include <seahorn/seahorn.h>
#include "trusty_ipc.h"

int main(void) {

  handle_t ports[4];

  ports[0] =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  sassert(ports[0] == 2);

  ports[1] =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- no more non-secure port handles
  sassert(ports[1] == INVALID_IPC_HANDLE);

  ports[2] = port_create("seahorn.com", 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- expected secure handle handle
  sassert(ports[2] == 1);

  ports[3] = port_create("seahorn.com", 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- no more secure handles
  sassert(ports[3] == INVALID_IPC_HANDLE);

  int rc;

  // release handle
  for (int i = 0; i < 4; ++i) {
    rc = close(ports[i]);
    sassert(rc == 0);
    ports[i] = INVALID_IPC_HANDLE;
  }

  // check if a new port can be created after close
  ports[0] =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  sassert(ports[0] == 2);

  return 0;
}
