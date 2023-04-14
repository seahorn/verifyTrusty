/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <assert.h>
#include <lk/list.h>
#include <stdlib.h>
#include <trusty_ipc.h>
#include <trusty_log.h>
#include <uapi/err.h>

#include <ipc.h>

#include "seahorn/seahorn.h"
#include <sea_handle_table.h>
#include <sea_ipc_helper.h>

#include "tipc_limits.h"
#include <interface/storage/storage.h>

/**
   entry point
*/
int main(void) {
  // handle_table_init(INVALID_IPC_HANDLE, INVALID_IPC_HANDLE,
  // INVALID_IPC_HANDLE);
  struct ipc_port_context *ctx = create_port_context();
  int rc =
      ipc_port_create(ctx, STORAGE_DISK_PROXY_PORT, 1, STORAGE_MAX_BUFFER_SIZE,
                      IPC_PORT_ALLOW_TA_CONNECT | IPC_PORT_ALLOW_NS_CONNECT);

  if (rc < 0) {
    return rc;
  }
#if HANDLE_TYPE_IS_PTR
  on_waitany_return(ctx->common.handle);
#endif
  ipc_loop();

  ipc_port_destroy(ctx);
  return 0;
}
