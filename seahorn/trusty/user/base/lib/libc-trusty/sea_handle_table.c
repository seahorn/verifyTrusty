/**
   Implementation of table of handles to be used by IPC
 */

/** Global variables that keep current handle information */

#include "sea_handle_table.h"
#include <seahorn/seahorn.h>

#define ND __declspec(noalias)
extern handle_t ND nd_handle(void);

// TODO: increase number of handles as needed
#define PORT_HANDLE_MIN 1
#define PORT_HANDLE_MAX 2

// TODO: increase number of channels as needed
#define CHAN_HANDLE_MIN 16
#define CHAN_HANDLE_MAX 17

#define IS_PORT_IPC_HANDLE(h) (PORT_HANDLE_MIN <= h && h <= PORT_HANDLE_MAX)
#define IS_CHAN_IPC_HANDLE(h) (!IS_PORT_IPC_HANDLE(h))
#define IS_SECURE_IPC_HANDLE(h) ((h)&0x1)
#define IS_NONSECURE_IPC_HANDLE(h) (!IS_SECURE_HANDLE(h))

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

// -- number of active port handles
unsigned g_active_phandles = 0;

// -- port handle 1
bool g_phandle1_active = false;
void *g_phandle1_cookie = NULL;

// -- port handle 2
bool g_phandle2_active = false;
void *g_phandle2_cookie = NULL;

// -- number of active channel handles
unsigned g_active_chandles = 0;

// -- channel handle 1
bool g_chandle16_active = false;
void *g_chandle16_cookie = false;

// -- channel handle 2
bool g_chandle17_active = false;
void *g_chandle17_cookie = false;

/** Convenience macros to access fields.

    Only use these macros to access the fields so that the representation can be
    changed later without changing the rest of the code.
 */

#define PHANDLE(ID, FLD) g_phandle##ID##_##FLD
#define CHANDLE(ID, FLD) g_chandle##ID##_##FLD

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
handle_t sea_ht_new_port(bool secure) {
  handle_t h = s_first_available_port_handle(secure);
  if (h != INVALID_IPC_HANDLE)
    g_active_phandles++;
  return h;
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

handle_t sea_ht_new_channel(handle_t port) {
  (void)port;

  handle_t h = s_first_available_channel_handle();
  if (h != INVALID_IPC_HANDLE)
    g_active_chandles++;
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

void* sea_ht_get_cookie_port(handle_t handle) {
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

void* sea_ht_get_cookie_channel(handle_t handle) {
#define CASE(X)                                                                \
  case X:                                                                      \
    return CHANDLE(X, cookie);

  switch (handle) {
    CASE(16);
    CASE(17);
  }
  return NULL;
}

void sea_ht_set_cookie(handle_t handle, void* cookie) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    sea_ht_set_cookie_port(handle, cookie);
  } else {
    sea_ht_set_cookie_channel(handle, cookie);
  }
}

void* sea_ht_get_cookie(handle_t handle) {
  if (IS_PORT_IPC_HANDLE(handle)) {
    return sea_ht_get_cookie_port(handle);
  } else {
    return sea_ht_get_cookie_channel(handle);
  }
}
