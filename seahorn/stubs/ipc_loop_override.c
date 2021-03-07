/** Verify Trusty

    Implementation of ipc_loop()

 */
#include <ipc_helper.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <seahorn/seahorn.h>
#include <uapi/err.h>

void ipc_loop(void) {
    int rc;
    uevent_t event;

    // Unroll the while loop only iterating twice
    #pragma unroll 2
    while (true) {
        event.handle = INVALID_IPC_HANDLE;
        event.event = 0;
        event.cookie = NULL;
        rc = wait_any(&event, INFINITE_TIME);
        if (rc < 0) {
            break;
        }

        if (rc == NO_ERROR) { /* got an event */
            sea_dispatch_event(&event);
        }
    }
}
