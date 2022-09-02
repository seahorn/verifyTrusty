/**
   Implementation of table of handles to be used by IPC
 */

/** Global variables that keep current handle information */

#include "sea_handle_table.h"
#include <nondet.h>
#include <seahorn/seahorn.h>

#define NUM_PHANDLE 2
#define NUM_CHANDLE 2


struct handle {
  handle_t id;
  bool active;
  void *cookie;

  /* port only */
  char path;
  /* channel only */
  uint32_t msg_id;
  size_t msg_len;
};

/** Globals */
// Define a global handle table for crab
struct handle port_handle_1;
struct handle port_handle_2;
struct handle chan_handle_3;
struct handle chan_handle_4;

unsigned num_active_phandles = 0;
unsigned num_active_chandles = 0;

/**************************** Port operations *************************/
/**
  Init a port handle
*/
void init_port_handle(struct handle *ptr, handle_t id) {
  ptr->id = id;
  ptr->active = false;
  ptr->cookie = NULL;
  ptr->path = '\0';
}

/**
   Returns the first port handle that is not active

   Returns NULL if no port handle is available
 */
static struct handle *s_first_available_port_handle(bool secure) {
  // search for a new port
  struct handle *ptr = &port_handle_1;
  if ((IS_SECURE_IPC_HANDLE(1) == secure) && !ptr->active) {
    init_port_handle(ptr, 1);
    return ptr;
  }
  ptr = &port_handle_2;
  if ((IS_SECURE_IPC_HANDLE(2) == secure) && !ptr->active) {
    init_port_handle(ptr, 2);
    return ptr;
  }
  // if no port available
  return NULL;
}

/**
   Returns the first port handle that is not active
   make it as active and set the path by the first character of path
   FIXME: the implemantation of path is brittle, we only store / compare
          the first character of path.

   Returns INVALID_IPC_HANDLE if no port handle is available
 */
handle_t sea_ht_new_port(bool secure, const char *path) {
  // search for a new port
  struct handle *ptr = &port_handle_1;
  if ((IS_SECURE_IPC_HANDLE(1) == secure) && !ptr->active) {
    init_port_handle(ptr, 1);
    num_active_phandles++;
    ptr->active = true;
    ptr->path = path[0];
    return ptr->id;
  }
  ptr = &port_handle_2;
  if ((IS_SECURE_IPC_HANDLE(2) == secure) && !ptr->active) {
    init_port_handle(ptr, 2);
    num_active_phandles++;
    ptr->active = true;
    ptr->path = path[0];
    return ptr->id;
  }
  // if no port available
  return INVALID_IPC_HANDLE;
}

handle_t sea_ht_match_port(const char *path) {
  if (path) {
    struct handle *ptr = &port_handle_1;
    // NOTE: we only compare the first character
    if (path[0] == ptr->path) {
      return ptr->id;
    }
    ptr = &port_handle_2;
    if (path[0] == ptr->path) {
      return ptr->id;
    }
  }
  return INVALID_IPC_HANDLE;
}

int sea_ht_free_port(handle_t handle) {
  struct handle *ptr = &port_handle_1;
  if (handle == ptr->id) {
    ptr->active = false;
    --num_active_phandles;
    return NO_ERROR;
  }
  ptr = &port_handle_2;
  if (handle == ptr->id) {
    ptr->active = false;
    --num_active_phandles;
    return NO_ERROR;
  }
  return ERR_BAD_HANDLE;
}

/**
   Non-deterministically chooses an active port handle

   INVALID_IPC_HANDLE if no active handle is available
 */
handle_t sea_ht_choose_active_port_handle(void) {
  handle_t handle = nd_handle();
  assume(IS_PORT_IPC_HANDLE(handle));
  struct handle *ptr = &port_handle_1;
  if (handle == ptr->id && ptr->active) {
    return handle;
  }
  ptr = &port_handle_2;
  if (handle == ptr->id && ptr->active) {
    return handle;
  }
  return INVALID_IPC_HANDLE;
}

bool sea_ht_is_active_port(handle_t handle) {
  struct handle *ptr = &port_handle_1;
  if (handle == ptr->id) {
    return ptr->active;
  }
  ptr = &port_handle_2;
  if (handle == ptr->id) {
    return ptr->active;
  }
  return false;
}

int sea_ht_set_cookie_port(handle_t handle, void *cookie) {
  struct handle *ptr = &port_handle_1;
  if (handle == ptr->id) {
    ptr->cookie = cookie;
    return NO_ERROR;
  }
  ptr = &port_handle_2;
  if (handle == ptr->id) {
    ptr->cookie = cookie;
    return NO_ERROR;
  }
  return ERR_BAD_HANDLE;
}

void *sea_ht_get_cookie_port(handle_t handle) {
  struct handle *ptr = &port_handle_1;
  if (handle == ptr->id) {
    return ptr->cookie;
  }
  ptr = &port_handle_2;
  if (handle == ptr->id) {
    return ptr->cookie;
  }
  return NULL;
}

/**************************** Channel operations *************************/
/**
  Init a channel handle
*/
void init_channel_handle(struct handle *ptr, handle_t id) {
  ptr->id = id;
  ptr->active = false;
  ptr->cookie = NULL;
  ptr->path = '\0';
  ptr->msg_id = 0;
  ptr->msg_len = 0;
}

static struct handle *s_first_available_channel_handle(void) {
  // search for a new channel
  struct handle *ptr = &chan_handle_3;
  if (!ptr->active) {
    init_channel_handle(ptr, 3);
    return ptr;
  }
  ptr = &chan_handle_4;
  if (!ptr->active) {
    init_channel_handle(ptr, 4);
    return ptr;
  }

  // if no channel available
  return NULL;
}

handle_t sea_ht_new_channel(handle_t port) {
  // search for a new channel
  struct handle *ptr = &chan_handle_3;
  if (!ptr->active) {
    init_channel_handle(ptr, 3);
    num_active_chandles++;
    ptr->active = true;
    return ptr->id;
    ;
  }
  ptr = &chan_handle_4;
  if (!ptr->active) {
    init_channel_handle(ptr, 4);
    num_active_chandles++;
    ptr->active = true;
    return ptr->id;
    ;
  }
  return INVALID_IPC_HANDLE;
}

int sea_ht_free_channel(handle_t handle) {
  struct handle *ptr = &chan_handle_3;
  if (handle == ptr->id) {
    ptr->active = false;
    --num_active_chandles;
    return NO_ERROR;
  }
  ptr = &chan_handle_4;
  if (handle == ptr->id) {
    ptr->active = false;
    --num_active_chandles;
    return NO_ERROR;
  }
  return ERR_BAD_HANDLE;
}

/**
   Non-deterministically chooses an active channel handle

   INVALID_IPC_HANDLE if no active handle is available
 */
handle_t sea_ht_choose_active_channel_handle(void) {
  handle_t handle = nd_handle();
  assume(IS_CHAN_IPC_HANDLE(handle));
  struct handle *ptr = &chan_handle_3;
  if (handle == ptr->id && ptr->active) {
    return handle;
  }
  ptr = &chan_handle_4;
  if (handle == ptr->id && ptr->active) {
    return handle;
  }
  return INVALID_IPC_HANDLE;
}

int sea_ht_set_cookie_channel(handle_t handle, void *cookie) {
  struct handle *ptr = &chan_handle_3;
  if (handle == ptr->id) {
    ptr->cookie = cookie;
    return NO_ERROR;
  }
  ptr = &chan_handle_4;
  if (handle == ptr->id) {
    ptr->cookie = cookie;
    return NO_ERROR;
  }
  return ERR_BAD_HANDLE;
}

void *sea_ht_get_cookie_channel(handle_t handle) {
  struct handle *ptr = &chan_handle_3;
  if (handle == ptr->id) {
    return ptr->cookie;
  }
  ptr = &chan_handle_4;
  if (handle == ptr->id) {
    return ptr->cookie;
  }
  return NULL;
}

bool sea_ht_has_msg(handle_t chan_handle) {
  struct handle *ptr = &chan_handle_3;
  if (chan_handle == ptr->id) {
    return ptr->msg_id > INVALID_IPC_MSG_ID;
  }
  ptr = &chan_handle_4;
  if (chan_handle == ptr->id) {
    return ptr->msg_id > INVALID_IPC_MSG_ID;
  }
  return false;
}

uint32_t sea_ht_get_msg_id(handle_t chan_handle) {
  struct handle *ptr = &chan_handle_3;
  if (chan_handle == ptr->id) {
    return ptr->msg_id;
  }
  ptr = &chan_handle_4;
  if (chan_handle == ptr->id) {
    return ptr->msg_id;
  }
  return INVALID_IPC_MSG_ID;
}

void sea_ht_set_msg_id(handle_t chan_handle, uint32_t id) {
  struct handle *ptr = &chan_handle_3;
  if (chan_handle == ptr->id) {
    ptr->msg_id = id;
    return;
  }
  ptr = &chan_handle_4;
  if (chan_handle == ptr->id) {
    ptr->msg_id = id;
    return;
  }
}

size_t sea_ht_get_msg_len(handle_t chan_handle) {
  struct handle *ptr = &chan_handle_3;
  if (chan_handle == ptr->id) {
    return ptr->msg_len;
  }
  ptr = &chan_handle_4;
  if (chan_handle == ptr->id) {
    return ptr->msg_len;
  }
  // return 0 length if handle not found
  return 0;
}

void sea_ht_set_msg_len(handle_t chan_handle, size_t len) {
  struct handle *ptr = &chan_handle_3;
  if (chan_handle == ptr->id) {
    ptr->msg_len = len;
    return;
  }
  ptr = &chan_handle_4;
  if (chan_handle == ptr->id) {
    ptr->msg_len = len;
    return;
  }
}

void sea_ht_new_nd_msg(handle_t chan_handle) {
  struct handle *ptr = &chan_handle_3;
  if (chan_handle == ptr->id) {
    ptr->msg_id = nd_msg_id();
    ptr->msg_len = nd_msg_len();
    assume(ptr->msg_id > INVALID_IPC_MSG_ID);
    return;
  }
  ptr = &chan_handle_4;
  if (chan_handle == ptr->id) {
    ptr->msg_id = nd_msg_id();
    ptr->msg_len = nd_msg_len();
    assume(ptr->msg_id > INVALID_IPC_MSG_ID);
    return;
  }
}

/*************************** Miscellaneous operations *************************/
int sea_ht_set_cookie(handle_t handle, void *cookie) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    return sea_ht_set_cookie_port(handle, cookie);
  } else if (IS_CHAN_IPC_HANDLE(handle)) {
    return sea_ht_set_cookie_channel(handle, cookie);
  } else {
    return ERR_BAD_HANDLE;
  }
}

void *sea_ht_get_cookie(handle_t handle) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    return sea_ht_get_cookie_port(handle);
  } else if (IS_CHAN_IPC_HANDLE(handle)) {
    return sea_ht_get_cookie_channel(handle);
  } else {
    return NULL;
  }
}

int sea_ht_free(handle_t handle) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    return sea_ht_free_port(handle);
  } else if (IS_CHAN_IPC_HANDLE(handle)) {
    return sea_ht_free_channel(handle);
  } else {
    return ERR_BAD_HANDLE;
  }
}

handle_t sea_ht_choose_active_handle(void) {
  handle_t handle = sea_ht_choose_active_port_handle();
  if (handle != INVALID_IPC_HANDLE) {
    return handle;
  } else {
    return sea_ht_choose_active_channel_handle();
  }
}