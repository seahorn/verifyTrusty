#include <seahorn/seahorn.h>
#include <lib/tipc/tipc.h>
#include <trusty_ipc.h>

int main(void) {
  int rc;
  handle_t h = INVALID_IPC_HANDLE;
  char path[64];

  /* Make tipc_connect fail and check if handle is unchanged */
  rc = tipc_connect(&h, NULL);
  sassert(rc <= 0);
} 