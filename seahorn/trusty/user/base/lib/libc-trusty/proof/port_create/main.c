#include <seahorn/seahorn.h>
#include <trusty_ipc.h>

int main(void) {

  handle_t h =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- got expected handle
  sassert(h == 2);

  handle_t h2 =
      port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);

  // -- no more non-secure port handles
  sassert(h2 == INVALID_IPC_HANDLE);

  h2 = port_create("seahorn.com", 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- expected secure handle handle
  sassert(h2 == 1);

  handle_t h3 = port_create("seahorn.com", 1, 100, IPC_PORT_ALLOW_NS_CONNECT);
  // -- no more secure handles
  sassert(h3 == INVALID_IPC_HANDLE);


  // release handle
  close(h);
  h = INVALID_IPC_HANDLE;

  // request again
  h = port_create("seahorn.com", 1, 100,
                  IPC_PORT_ALLOW_NS_CONNECT | IPC_PORT_ALLOW_TA_CONNECT);
  sassert(h == 2);

  return 0;
}
