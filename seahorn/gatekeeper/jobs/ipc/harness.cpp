#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <trusty_ipc.h>
#include <uapi/err.h>

#include "gatekeeper_ipc.h"
#include "trusty_gatekeeper.h"

#include "seahorn/seahorn.h"

using namespace gatekeeper;
extern TrustyGateKeeper *device;

void gatekeeper_handle_port(uevent_t *ev);

void gatekeeper_handle_channel(uevent_t *ev);

long gatekeeper_ipc_init(void);

int main(void) {
  long rc;
  uevent_t event;

  TLOGI("Initializing\n");

  device = new TrustyGateKeeper();

  rc = gatekeeper_ipc_init();
  if (rc < 0) {
    TLOGE("failed (%ld) to initialize gatekeeper", rc);
    return rc;
  }

  handle_t port = (handle_t)rc;

/* enter main event loop */
  // Unroll the while loop only iterating twice
  int unroll_time = 0;
  while (unroll_time < 2) {
    event.handle = INVALID_IPC_HANDLE;
    event.event = 0;
    event.cookie = NULL;

    rc = wait_any(&event, INFINITE_TIME);
    if (rc < 0) {
      TLOGE("wait_any failed (%ld)\n", rc);
      break;
    }

    if (rc == NO_ERROR) { /* got an event */
      if (event.handle == port) {
        gatekeeper_handle_port(&event);
      } else {
        gatekeeper_handle_channel(&event);
      }
    }
    unroll_time ++;
  }

  return 0;
}