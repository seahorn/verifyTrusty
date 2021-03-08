#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <trusty_ipc.h>
#include <uapi/err.h>

#include "gatekeeper_ipc.h"
#include "trusty_gatekeeper.h"

#include "seahorn/seahorn.h"

using namespace gatekeeper;
TrustyGateKeeper *ver_device;

extern gatekeeper_error_t tipc_err_to_gatekeeper_err(long tipc_err);

extern gatekeeper_error_t handle_request(uint32_t cmd, uint8_t *in_buf,
                                         uint32_t in_buf_size,
                                         UniquePtr<uint8_t[]> *out_buf,
                                         uint32_t *out_buf_size);

extern gatekeeper_error_t send_response(handle_t chan, uint32_t cmd,
                                        uint8_t *out_buf,
                                        uint32_t out_buf_size);

extern gatekeeper_error_t send_error_response(handle_t chan, uint32_t cmd,
                                              gatekeeper_error_t err);

extern void gatekeeper_handle_port(uevent_t *ev);

extern void gatekeeper_handle_channel(uevent_t *ev);

extern long gatekeeper_ipc_init(void);

int main(void) {
  long rc;
  uevent_t event;

  TLOGI("Initializing\n");

  ver_device = new TrustyGateKeeper();

  rc = gatekeeper_ipc_init();
  if (rc < 0) {
    TLOGE("failed (%ld) to initialize gatekeeper", rc);
    return rc;
  }

  handle_t port = (handle_t)rc;

/* enter main event loop */
#pragma unroll 2
  while (true) {
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
  }

  return 0;
}