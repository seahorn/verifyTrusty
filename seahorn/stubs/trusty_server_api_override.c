#include "handle_table.h"
#include "nondet.h"
#include "sea_mem_helper.h"
#include "seahorn/seahorn.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

// trusty reference for definitions only
#include <trusty_ipc.h> // -> ipc structs
#include <uapi/err.h>   // NO_ERROR definition

/* Creates a named service port. */
handle_t port_create(const char *path, uint32_t num_recv_bufs,
                     uint32_t recv_buf_size, uint32_t flags) {
  (void)path;
  (void)num_recv_bufs;
  (void)recv_buf_size;
  handle_t retval = nd_port_handle();
  if (retval < 0)
    return retval; // return error message as is

  retval |= 0x2; // is port, set 2nd bit to 1

  if ((flags & IPC_PORT_ALLOW_TA_CONNECT) &&
      !(flags & IPC_PORT_ALLOW_NS_CONNECT)) {
    // open secure port only, set 1st bit to 1
    retval |= 0x1;
  } else if (!(flags & IPC_PORT_ALLOW_TA_CONNECT) &&
             (flags & IPC_PORT_ALLOW_NS_CONNECT)) {
    // open non secure port only, set 1st bit to 0
    retval &= ~(0x1);
  }
  add_handle(retval);
  return retval;
}

/** Accepts an incoming connection and gets a handle to a channel. */
handle_t accept(handle_t port_handle, uuid_t *peer_uuid) {
  (void)port_handle;
  handle_t chan = nd_chan_handle();
  if (chan >= 0) {
    assume(!(chan & 0x2)); // is channel
    // define peer_uuid to a dummy value
    peer_uuid->time_low = nd_time_low();
    peer_uuid->time_mid = nd_time_mid();
    peer_uuid->time_hi_and_version = nd_time_hi_n_ver();
    add_handle(chan);
  }
  return chan;
}
