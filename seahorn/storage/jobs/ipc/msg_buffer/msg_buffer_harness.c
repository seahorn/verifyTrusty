#include <assert.h>
#include <lk/list.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <trusty_log.h>
#include <uapi/err.h>

#include <ipc.h>

#include "tipc_limits.h"
#include <interface/storage/storage.h>

#include "handle_table.h"
#include "seahorn/seahorn.h"
#include "sea_ipc_helper.h"

/**
   verification entry point
 */
int main(void) {
  // handle_table_init(INVALID_IPC_HANDLE, INVALID_IPC_HANDLE, INVALID_IPC_HANDLE);
  struct ipc_port_context ctx = {
      .ops = {.on_connect = sea_sync_channel_connect},
  };
  // struct ipc_port_context* ctx = create_sync_port_context();
  int rc =
      ipc_port_create(&ctx, STORAGE_DISK_PROXY_PORT, 1, STORAGE_MAX_BUFFER_SIZE,
                      IPC_PORT_ALLOW_TA_CONNECT | IPC_PORT_ALLOW_NS_CONNECT);

  if (rc < 0) {
    return rc;
  }

  ipc_loop();

  ipc_port_destroy(&ctx);

  return 0;
}
