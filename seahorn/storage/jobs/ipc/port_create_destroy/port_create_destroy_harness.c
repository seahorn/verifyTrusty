#include <assert.h>
#include <lk/list.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <trusty_log.h>
#include <uapi/err.h>

#include "ipc.h"

// #include "handle_table.h"
#include "sea_handle_table.h"
#include "seahorn/seahorn.h"
#include "sea_ipc_helper.h"

#include "tipc_limits.h"
#include <interface/storage/storage.h>

/** Test harness entry point */
int main(void) {
  /* initialize handle table */
  // handle_table_init(INVALID_IPC_HANDLE, INVALID_IPC_HANDLE, INVALID_IPC_HANDLE);

  /*  setup port context */
  struct ipc_port_context* ctx = create_port_context();

  /*  call ipc_port_create */
  int rc =
      ipc_port_create(ctx, STORAGE_DISK_PROXY_PORT, 1, STORAGE_MAX_BUFFER_SIZE,
                      IPC_PORT_ALLOW_TA_CONNECT | IPC_PORT_ALLOW_NS_CONNECT);

  /*  bail out if error */
  if (rc < 0) {
    return rc;
  }

  /*  check that handle is registered if connection succeeds */
  sassert(sea_ht_is_active_port(ctx->common.handle));

  // ipc_loop();

  /*  destroy port */
  ipc_port_destroy(ctx);

  /*  check that handle is unregistered properly */
  sassert(!sea_ht_is_active_port(ctx->common.handle));

  return 0;
}
