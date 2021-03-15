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

/* Waits for an event to occur on a given handle for specified period of time.
"[retval]: NO_ERROR if a valid event occurred within a specified timeout
interval; ERR_TIMED_OUT if a specified timeout elapsed but no event has
occurred; < 0 for other errors"
*/
int wait(handle_t handle, uevent_t *event, uint32_t timeout_msecs) {
  (void)timeout_msecs;
  int ret = nd_wait_ret();
  assume(ret == NO_ERROR || ret < 0);
  if (ret == NO_ERROR) {
    event->handle = handle;
    event->cookie = get_handle_cookie(event->handle);
    event->event = nd_event_flag();
    assume(event->event < (uint32_t)0x16); // max is (1111)2
  }
  return ret;
}

/* Wait for any kind of event, could be port or channel

   This implementation only handles the channel event of the latest port event
   if they happen to match.

   Update: assume if success, only returns handles currently on the table
*/
int wait_any(uevent_t *ev, uint32_t timeout_msecs) {
  (void)timeout_msecs;
  handle_t active_handle;
  uint32_t event_flag;
  // -- chose a handle of a currently active channel or port. break ties.
  if (is_current_chan_active()) {
    active_handle = get_current_chan_handle();
  } else if (is_secure_port_active()) {
    active_handle = get_secure_port_handle();
  } else if (is_non_secure_port_active()) {
    active_handle = get_non_secure_port_handle();
  } else {
    active_handle = nd_wait_handle();
  }
  ev->handle = active_handle;
  ev->cookie = get_handle_cookie(ev->handle);

  event_flag = nd_event_flag();
  assume(event_flag < (uint32_t)0x16); // max is (1111)2
  ev->event = event_flag;

  int ret = nd_wait_any_ret();
  assume(ret <= NO_ERROR);
  return ret;
}

/** Associates the caller-provided private data with a specified handle. */
int set_cookie(handle_t handle, void *cookie) {
  // the handle should at least be stored in the handle table?
  // similar check can be seen in trusty/kernel/lib/trusty/uctx.c
  // sassert(contains_handle(handle));
  if (!contains_handle(handle)) {
    return -1;
  }

  int ret = nd_set_cookie_ret(); // model other results (including failure)
  assume(ret <= 0); // NO_ERROR on success, < 0 error code otherwise
  if (ret == 0) {
    set_handle_cookie(handle, cookie);
  }
  return ret;
}

/** Destroys the resource associated with the specified handle and removes it
   from the handle table. */
int close(handle_t handle) {
  int ret = nd_close_ret();
  assume(ret <= 0); // "0 if success; a negative error otherwise"
  remove_handle(handle);
  return ret;
}
