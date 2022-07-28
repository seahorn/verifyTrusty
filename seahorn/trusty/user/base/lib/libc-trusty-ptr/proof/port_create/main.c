#include "trusty_ipc.h"
#include <seahorn/seahorn.h>

const char *PATH = "seahorn.com";

int main(void) {

  handle_t h = port_create(
      PATH, 1, 100, IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  sassert(!h->secure);

  handle_t h2 = port_create(
      PATH, 1, 100, IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- can create more insecure handles
  sassert(!h2->secure);

  h2 = port_create(PATH, 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- expected secure handle handle
  sassert(h2->secure);

  handle_t h3 = port_create(PATH, 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- can create more secure handles
  sassert(h3->secure);

  // release handle
  close(h);
  h = INVALID_IPC_HANDLE;

  // request again
  h = port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);
  sassert(!h->secure);
  return 0;
}
