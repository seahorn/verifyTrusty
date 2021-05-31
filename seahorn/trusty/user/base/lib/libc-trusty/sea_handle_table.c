/**
   Implementation of table of handles to be used by IPC
 */

/** Global variables that keep current handle information */

#include "sea_handle_table.h"
#include <seahorn/seahorn.h>

#define ND __declspec(noalias)
extern handle_t ND nd_handle(void);

/**
   ID DESCRIPTION
   1  port handle secure 1
   2  port handle non-secure 1
   3  port handle secure 2
   4  port handle non-secure 2
   5  port handle secure 3
   6  port handle non-secure 3
   7  port handle secure 4
   8  port handle non-secure 4

   16 channel handle 1
   17 channel handle 2
   18 channel handle 3
   19 channel handle 4
 */

/** GLOBALS */

/*
  Each handle in the table is represented by a collection of global variables.
  This flat representation is easiest to model, but is quite cumbersome to work
  with.

  We may move to representing each handle by a struct, and, maybe, set of
  handles by an array.

  But for now, prefer simplicity of the representation
 */

/** Convenience macros to access fields.

    Only use these macros to access the fields so that the representation can be
    changed later without changing the rest of the code.
 */

#define PHANDLE(ID, FLD) g_phandle##ID##_##FLD
#define CHANDLE(ID, FLD) g_chandle##ID##_##FLD

// -- number of active port handles
unsigned g_active_phandles = 0;

#define HANDLE_DEF(ID)                                                         \
  bool PHANDLE(ID, active) = false;                                            \
  void *PHANDLE(ID, cookie) = NULL;                                            \
  char PHANDLE(ID, path) = '\0';

// -- port handle 1
HANDLE_DEF(1)
// -- port handle 2
HANDLE_DEF(2);

// -- number of active channel handles
unsigned g_active_chandles = 0;

#define CHAN_DEF(ID)                                                           \
  bool CHANDLE(ID, active) = false;                                            \
  void *CHANDLE(ID, cookie) = NULL;                                            \
  uint32_t CHANDLE(ID, msg_id) = 0;                                            \
  size_t CHANDLE(ID, msg_len) = 0;

// -- channel handle 1
CHAN_DEF(16)
// -- channel handle 2
CHAN_DEF(17)

bool sea_ht_has_msg(handle_t chan_handle) {
#define CASE(X)                                                                \
  case X:                                                                      \
    return CHANDLE(X, msg_id) > 0;

  switch (chan_handle) {
    CASE(16);
    CASE(17);
  }
  return false;
}

uint32_t sea_ht_get_msg_id(handle_t chan_handle) {
#define CASE(X)                                                                \
  case X:                                                                      \
    return CHANDLE(X, msg_id);
  switch (chan_handle) {
    CASE(16);
    CASE(17);
  }
  return 0;
}

void sea_ht_set_msg_id(handle_t chan_handle, uint32_t id) {
#define CASE(X)                                                                \
  case X:                                                                      \
    CHANDLE(X, msg_id) = id;                                                   \
    return;
  switch (chan_handle) {
    CASE(16);
    CASE(17);
  }
}

size_t sea_ht_get_msg_len(handle_t chan_handle) {
#define CASE(X)                                                                \
  case X:                                                                      \
    return CHANDLE(X, msg_len);
  switch (chan_handle) {
    CASE(16);
    CASE(17);
  }
  return 0;
}

void sea_ht_set_msg_len(handle_t chan_handle, size_t len) {
#define CASE(X)                                                                \
  case X:                                                                      \
    CHANDLE(X, msg_len) = len;                                                 \
    return;
  switch (chan_handle) {
    CASE(16);
    CASE(17);
  }
}

extern ND size_t nd_msg_len(void);
extern ND uint32_t nd_msg_id(void);
void sea_ht_new_nd_msg(handle_t chan_handle) {
#define CASE(X)                                                                \
  case X:                                                                      \
    CHANDLE(X, msg_len) = nd_msg_len();                                        \
    CHANDLE(X, msg_id) = nd_msg_id();                                          \
    assume(CHANDLE(X, msg_id) > INVALID_IPC_MSG_ID);                           \
    return;

  switch (chan_handle) {
    CASE(16);
    CASE(17);
  }
}

/**
   Returns the first port handle that is not active

   Returns INVALID_IPC_HANDLE if no port handle is available
 */
static handle_t s_first_available_port_handle(bool secure) {
#define CASE(X)                                                                \
  if ((IS_SECURE_IPC_HANDLE(X) == secure) && !PHANDLE(X, active))              \
    return X;

  CASE(1);
  CASE(2);

  // ...
  return INVALID_IPC_HANDLE;
}

/**
   Frees a handle by removing it from the table

   The handle is can be reused
 */
void sea_ht_free(handle_t handle) {
#define CASE(X)                                                                \
  case X:                                                                      \
    PHANDLE(X, active) = false;                                                \
    --g_active_phandles;                                                       \
    break;

#define CCASE(X)                                                               \
  case X:                                                                      \
    CHANDLE(X, active) = false;                                                \
    --g_active_chandles;                                                       \
    break;

  switch (handle) {
    CASE(1);
    CASE(2);
    CCASE(16);
    CCASE(17);
  }
}

/**
   Allocates a new port handle

   Return INVALID_IPC_HANDLE if no handle is available to be allocated
 */
handle_t sea_ht_new_port(bool secure, const char *path) {
#define CASE(X)                                                                \
  case X:                                                                      \
    PHANDLE(X, active) = true;                                                 \
    PHANDLE(X, path) = path[0];                                                \
    return X;

  handle_t h = s_first_available_port_handle(secure);
  if (h == INVALID_IPC_HANDLE)
    return h;
  g_active_phandles++;
  switch (h) {
    CASE(1)
    CASE(2)
  }
  return INVALID_IPC_HANDLE;
}

handle_t sea_ht_math_port(const char *path) {
#define CASE(X)                                                                \
  if (path && path[0] == PHANDLE(X, path))                                     \
    return X;
  CASE(1);
  CASE(2);
  return INVALID_IPC_HANDLE;
}

/**
   Non-deterministically chooses an active handle

   Blocks if no active handle is available
 */
handle_t sea_ht_choose_active_handle(void) {
#define CASE(X)                                                                \
  case X:                                                                      \
    assume(PHANDLE(X, active));                                                \
    return X;
#define CCASE(X)                                                               \
  case X:                                                                      \
    assume(CHANDLE(X, active));                                                \
    return X;

  handle_t v = nd_handle();
  assume((PORT_HANDLE_MIN <= v && v <= PORT_HANDLE_MAX) ||
         (CHAN_HANDLE_MIN <= v && v <= CHAN_HANDLE_MAX));

  switch (v) {
    CASE(1);
    CASE(2);
    CCASE(16);
    CCASE(17);
  }
  return INVALID_IPC_HANDLE;
}

static handle_t s_first_available_channel_handle(void) {
#define CASE(X)                                                                \
  if (!CHANDLE(X, active))                                                     \
    return X;

  CASE(16);
  CASE(17);

  // ...
  return INVALID_IPC_HANDLE;
}

handle_t sea_ht_new_channel(handle_t parent_port) {
#define CASE(X)                                                                \
  case X:                                                                      \
    CHANDLE(X, active) = true;                                                 \
    return X;

  handle_t h = s_first_available_channel_handle();
  if (h == INVALID_IPC_HANDLE)
    return h;
  g_active_chandles++;
  switch (h) {
    CASE(16)
    CASE(17)
  }
  return INVALID_IPC_HANDLE;
}

bool sea_ht_is_active_port(handle_t handle) {
#define CASE(X)                                                                \
  case X:                                                                      \
    return PHANDLE(X, active);

  switch (handle) {
    CASE(1);
    CASE(2);
  }
  return false;
}

void sea_ht_set_cookie_port(handle_t handle, void *cookie) {
#define CASE(X)                                                                \
  case X:                                                                      \
    PHANDLE(X, cookie) = cookie;                                               \
    break;

  switch (handle) {
    CASE(1);
    CASE(2);
  }
}

void *sea_ht_get_cookie_port(handle_t handle) {
#define CASE(X)                                                                \
  case X:                                                                      \
    return PHANDLE(X, cookie);

  switch (handle) {
    CASE(1);
    CASE(2);
  }
  return NULL;
}

void sea_ht_set_cookie_channel(handle_t handle, void *cookie) {
#define CASE(X)                                                                \
  case X:                                                                      \
    CHANDLE(X, cookie) = cookie;                                               \
    break;

  switch (handle) {
    CASE(16);
    CASE(17);
  }
}

void *sea_ht_get_cookie_channel(handle_t handle) {
#define CASE(X)                                                                \
  case X:                                                                      \
    return CHANDLE(X, cookie);

  switch (handle) {
    CASE(16);
    CASE(17);
  }
  return NULL;
}

void sea_ht_set_cookie(handle_t handle, void *cookie) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    sea_ht_set_cookie_port(handle, cookie);
  } else {
    sea_ht_set_cookie_channel(handle, cookie);
  }
}

void *sea_ht_get_cookie(handle_t handle) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    return sea_ht_get_cookie_port(handle);
  } else {
    return sea_ht_get_cookie_channel(handle);
  }
}
