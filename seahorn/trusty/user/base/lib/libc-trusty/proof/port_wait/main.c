#include <seahorn/seahorn.h>
#include <trusty_ipc.h>
#include <uapi/err.h>

int main(void) {

  handle_t port =
      port_create("ta.seahorn.com", 1, 100, IPC_PORT_ALLOW_TA_CONNECT);

  // expect non-secure handle
  sassert(port == 2);

  uevent_t event;

  event.handle = INVALID_IPC_HANDLE;
  event.event = 0;
  event.cookie = NULL;

  handle_t rc = wait(port, &event, 0);
  sassert(rc <= NO_ERROR);
  if (rc == NO_ERROR) {
    /* got an event */
    sassert(event.handle == port);
    // No cookie set
    sassert(!event.cookie);
  }

  return 0;
}
