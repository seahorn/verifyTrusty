#include <seahorn/seahorn.h>
#include <trusty_ipc.h>
#include <uapi/err.h>

typedef void (*event_handler_proc_t)(const uevent_t* ev, void* ctx);

struct ipc_event_handler {
    event_handler_proc_t proc;
    void* priv;
};

void port_handler(const uevent_t* ev, void* ctx) {
  return;
}

static struct ipc_event_handler port_evt_handler = {
  .proc = port_handler,
  .priv = NULL,
};

int main(void) {

  handle_t port =
      port_create("ta.seahorn.com", 1, 100, IPC_PORT_ALLOW_TA_CONNECT);

  // expect non-secure handle
  sassert(port == 2);

  uevent_t event;
  handle_t rc;

  rc = set_cookie(port, &port_evt_handler);

  event.handle = INVALID_IPC_HANDLE;
  event.event = 0;
  event.cookie = NULL;

  rc = wait(port, &event, 0);
  sassert(rc <= NO_ERROR);
  if (rc == NO_ERROR) {
    /* got an event */
    sassert(event.handle == port);
    // Cookie set
    sassert(event.cookie);
  }

  return 0;
}
