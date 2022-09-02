#include "sea_handle_table.h"
#include "trusty_syscalls.h"

#include <uapi/err.h> // trusty errors definitions
#include <nondet.h>
#include <seahorn/seahorn.h>

/******************************** Server APIs *********************************/

/* Documentation from trusty API:
  port_create()
Creates a named service port.

[in] path: The string name of the port (as described above). This name should be unique across the system; attempts to create a duplicate will fail.
[in] num_recv_bufs: The maximum number of buffers that a channel on this port '
can pre-allocate to facilitate the exchange of data with the client. 
Buffers are counted separately for data going in both directions, 
so specifying 1 here would mean 1 send and 1 receive buffer are preallocated.
[in] recv_buf_size: Maximum size of each individual buffer in the above buffer set. 
This value is protocol-dependent and effectively limits maximum message size you can exchange with peer
[in] flags: A combination of flags that specifies additional port behavior

[retval]: Handle to the port created if non-negative or a specific error if negative
*/
handle_t _trusty_port_create(const char *path, uint32_t num_recv_bufs,
                             uint32_t recv_buf_size, uint32_t flags) {
  (void)path;

  if (num_recv_bufs == 0 || recv_buf_size == 0) {
    // error if no buffers or zero buf size
    return ERR_INVALID_ARGS; 
  }

  if (!path) return ERR_BAD_PATH; // error if path not exists

  // IPC_PORT_ALLOW_TA_CONNECT - allows a connection from other secure apps
  // IPC_PORT_ALLOW_NS_CONNECT - allows a connection from the non-secure world
  bool secure = (flags & IPC_PORT_ALLOW_NS_CONNECT) &&
                !(flags & IPC_PORT_ALLOW_TA_CONNECT);
  return sea_ht_new_port(secure, path);
}

/* Documentation from trusty API:
  accept()
Accepts an incoming connection and gets a handle to a channel.

[in] handle_id: Handle representing the port to which a client has connected
[out] peer_uuid: Pointer to a uuud_t structure to be filled with the UUID of 
  the connecting client application. It will be set to all zeros if the 
  connection originated from the non-secure world.

[retval]: Handle to a channel (if non-negative) on which the server can exchange 
  messages with the client (or an error code otherwise)
*/
handle_t _trusty_accept(handle_t port_handle, uuid_t *peer_uuid) {
  (void)port_handle;
  handle_t chan = sea_ht_new_channel(port_handle);
  if (chan != INVALID_IPC_HANDLE) {
    if ((port_handle & IPC_PORT_ALLOW_TA_CONNECT) == 0) {
      // non-secure world
      peer_uuid->time_low = 0;
      peer_uuid->time_mid = 0;
      peer_uuid->time_hi_and_version = 0;
    }
    else {
      // define peer_uuid to a dummy value
      peer_uuid->time_low = nd_time_low();
      peer_uuid->time_mid = nd_time_mid();
      peer_uuid->time_hi_and_version = nd_time_hi_n_ver();
    }
  }
  return chan;
}


/******************************** Client APIs *********************************/
/* Documentation from trusty API:
  connect()
Initiates a connection to a port specified by name.

[in] path: Name of a port published by a Trusty application
[in] flags: Specifies additional, optional behavior

[retval]: Handle to a channel over which messages can be exchanged 
with the server; error if negative
*/
handle_t _trusty_connect(const char *path, uint32_t flags) {
  handle_t port_handle = sea_ht_match_port(path);
  if (port_handle != INVALID_IPC_HANDLE) {
    // if a port exists, create a new channel
    return sea_ht_new_channel(port_handle);
  }
  return INVALID_IPC_HANDLE;
}

/******************************** Handle APIs *********************************/
/* Documentation from trusty API:
  set_cookie()
Associates the caller-provided private data with a specified handle.

[in] handle: Any handle returned by one of the API calls
[in] cookie: Pointer to arbitrary user-space data in the Trusty application

[retval]: NO_ERROR on success, error if negative
*/
int _trusty_set_cookie(handle_t handle, void *cookie) {
  return sea_ht_set_cookie(handle, cookie);
}

/* Documentation from trusty API:
  wait()
Waits for an event to occur on a given handle for specified period of time.

[in] handle_id: Any handle returned by one of the API calls
[in] timeout_msecs: A timeout value in milliseconds; a value of -1 is an infinite timeout
[out] event: A pointer to the structure representing an event that occurred on this handle

[retval]: NO_ERROR if a valid event occurred within a timeout interval; 
ERR_TIMED_OUT if a specified timeout elapsed but no event has occurred; 
< 0 for other errors
*/
int _trusty_wait(handle_t handle, struct uevent *event,
                 uint32_t timeout_msecs) {
  (void)timeout_msecs;

  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;

  if (handle == INVALID_IPC_HANDLE)
    return handle;

  event->handle = handle;
  // NOTE: cookie may be null
  event->cookie = sea_ht_get_cookie(handle);
  event->event = nd_trusty_ipc_event();

  if (IS_PORT_IPC_HANDLE(handle)) {
    event->event = IPC_HANDLE_POLL_READY;
  }

  // Upon success (retval == NO_ERROR), fills a specified uevent_t structure 
  // with information about the event that occurred.
  if (IS_CHAN_IPC_HANDLE(handle)) {
    if (event->event & IPC_HANDLE_POLL_MSG) {
      // IPC_HANDLE_POLL_MSG indicates that 
      // there is a pending message for this channel
      if (sea_ht_get_msg_id(handle) == INVALID_IPC_MSG_ID) {
        // pretend a message sent
        sea_ht_new_nd_msg(handle);
      }
    }
  }

  return NO_ERROR;
}

/*
  wait_any()
Waits for an event to occur on any handles for specified period of time.

[in] timeout_msecs: A timeout value in milliseconds; a value of -1 is an infinite timeout
[out] event: A pointer to the structure representing an event that occurred on some handle

[retval]: NO_ERROR if a valid event occurred within a timeout interval; 
ERR_TIMED_OUT if a specified timeout elapsed but no event has occurred; 
< 0 for other errors
*/
int _trusty_wait_any(uevent_t *ev, uint32_t timeout_msecs) {
  (void)timeout_msecs;

  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;

  handle_t h = sea_ht_choose_active_handle();
  if (h == INVALID_IPC_HANDLE)
    return h;

  ev->handle = h;
  // NOTE: cookie may be null
  ev->cookie = sea_ht_get_cookie(h);
  ev->event = nd_trusty_ipc_event();

  if (IS_PORT_IPC_HANDLE(h)) {
    ev->event = IPC_HANDLE_POLL_READY;
  }

  // Upon success (retval == NO_ERROR), fills a specified uevent_t structure 
  // with information about the event that occurred.
  if (IS_CHAN_IPC_HANDLE(h)) {
    if (ev->event & IPC_HANDLE_POLL_MSG) {
      // IPC_HANDLE_POLL_MSG indicates that 
      // there is a pending message for this channel
      if (sea_ht_get_msg_id(h) == INVALID_IPC_MSG_ID) {
        // pretend a message sent
        sea_ht_new_nd_msg(h);
      }
    }
  }

  return NO_ERROR;
}
/* Documentation from trusty API:
  close()
Destroys the resource associated with the specified handle and removes it from the handle table.

[in] handle_id: Handle to destroy

[retval]: 0 if success; a negative error otherwise
*/

int _trusty_close(handle_t handle) {
  return sea_ht_free(handle);
}

handle_t _trusty_handle_set_create(void) { return INVALID_IPC_HANDLE; }

int _trusty_handle_set_ctrl(handle_t handle, uint32_t cmd, struct uevent *evt) {
  return ERR_GENERIC;
}

/******************************** Message APIs ********************************/
/* Documentation from trusty API:
  send_msg()
Sends a message over a specified channel.

[in] handle: Handle to the channel over which to send the message.
[in] msg: Pointer to the ipc_msg_t structure describing the message.

[retval]: Total number of bytes sent on success; a negative error otherwise.
*/
ssize_t _trusty_send_msg(handle_t handle, struct ipc_msg *msg) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;
  if (handle == INVALID_IPC_HANDLE || IS_PORT_IPC_HANDLE(handle))
    return ERR_INVALID_ARGS;
  if (!msg)
    return ERR_FAULT;

  sassert(0 <= msg->num_iov);
  sassert(msg->num_iov <= MAX_IPC_MSG_NUM);

  size_t sent_bytes = 0;
  sea_ht_new_nd_msg(handle);
  for (size_t i = 0; i < msg->num_iov; ++i) {
    sent_bytes += msg->iov[i].iov_len;
    sassert(sea_is_dereferenceable(msg->iov[i].iov_base, msg->iov[i].iov_len));
  }
  sea_ht_set_msg_len(handle, sent_bytes);
  return sent_bytes;
}

/* Documentation from trusty API:
  get_msg()
Gets meta-information about the next message in an incoming message queue
of a specified channel.

[in] handle: Handle of the channel on which a new message must be retrieved.
[out] msg_info: Message information structure described as follows:
typedef struct ipc_msg_info {
        size_t    len;
        uint32_t  id;
} ipc_msg_info_t;
Each message is assigned a unique ID across the set of outstanding messages, 
and the total length of each message is filled in. 
If configured and allowed by the protocol, 
there can be multiple outstanding (opened) messages at once for a particular channel.

[retval]: NO_ERROR on success; a negative error otherwise
*/
int _trusty_get_msg(handle_t handle, struct ipc_msg_info *msg_info) {
  if (handle == INVALID_IPC_HANDLE || IS_PORT_IPC_HANDLE(handle))
    // get message on invalid handle
    return ERR_INVALID_ARGS;
  if (!msg_info) {
    return ERR_GENERIC;
  }
  if (sea_ht_get_msg_id(handle) == INVALID_IPC_MSG_ID) {
    return ERR_NO_MSG;
  }
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;

  msg_info->id = sea_ht_get_msg_id(handle);
  msg_info->len = sea_ht_get_msg_len(handle);

  return msg_info->id != INVALID_IPC_MSG_ID ? NO_ERROR : INVALID_IPC_MSG_ID;
}

/* Documentation from trusty API:
  read_msg()
Reads the content of the message with the specified ID starting from the 
specified offset.

[in] handle: Handle of the channel from which to read the message.
[in] msg_id: ID of the message to read.
[in] offset: Offset into the message from which to start reading.
[in] msg: Pointer to the ipc_msg_t structure describing a set of buffers into 
which to store incoming message data.

[retval]: Total number of bytes stored in the dst buffers on success; 
a negative error otherwise.
*/
ssize_t _trusty_read_msg(handle_t handle, uint32_t msg_id, uint32_t offset,
                         struct ipc_msg *msg) {
  if (handle == INVALID_IPC_HANDLE || IS_PORT_IPC_HANDLE(handle))
    // read message on invalid handle
    return ERR_INVALID_ARGS;

  if (sea_ht_get_msg_id(handle) != msg_id)
    // read message on invalid msg id
    return ERR_INVALID_ARGS;

  if (!msg)
    // read message on invalid msg
    return ERR_FAULT;
  
  // if no msg on this channel
  if (sea_ht_get_msg_id(handle) == INVALID_IPC_MSG_ID) {
    return ERR_NO_MSG;
  }

  sassert(0 <= msg->num_iov);
  sassert(msg->num_iov <= MAX_IPC_MSG_NUM);

  size_t msg_len = sea_ht_get_msg_len(handle);
  if (msg_len < 0)
    // although this should not happened, just in case
    return ERR_GENERIC;

  if (offset > msg_len)
    // read with invalid offset
    return ERR_INVALID_ARGS;
  sassert(offset <= msg_len);

  msg_len -= offset;

  size_t num_bytes_read = 0;
  #pragma unroll MAX_IPC_MSG_NUM
  for (size_t i = 0; i < msg->num_iov; ++i) {
    if (msg_len < msg->iov[i].iov_len) {
      sassert(sea_is_dereferenceable(msg->iov[i].iov_base, msg_len));
      memhavoc(msg->iov[i].iov_base, msg_len);
      num_bytes_read += msg_len;
      return num_bytes_read;
    }
    sassert(sea_is_dereferenceable(msg->iov[i].iov_base,  msg->iov[i].iov_len));
    memhavoc(msg->iov[i].iov_base, msg->iov[i].iov_len);
    num_bytes_read += msg->iov[i].iov_len;
    // avoid msg_len underflow
    if (msg->iov[i].iov_len < msg_len) {
      msg_len -= msg->iov[i].iov_len;
    } else {
      msg_len = 0;
    }
  }

  return num_bytes_read;
}

/* Documentation from trusty API:
  put_msg()
Retires a message with a specified ID.

[in] handle: Handle of the channel on which the message has arrived.
[in] msg_id: ID of message being retired.

[retval]: NO_ERROR on success; a negative error otherwise.
*/
int _trusty_put_msg(handle_t handle, uint32_t msg_id) {
  int err = nd_trusty_ipc_err();
  if (err < NO_ERROR)
    return err;

  if (handle == INVALID_IPC_HANDLE || IS_PORT_IPC_HANDLE(handle))
    // read message on invalid handle
    return ERR_INVALID_ARGS;

  if (sea_ht_get_msg_id(handle) != msg_id)
    // read message on invalid msg id
    return ERR_INVALID_ARGS;

  sea_ht_set_msg_id(handle, INVALID_IPC_MSG_ID);
  sea_ht_set_msg_len(handle, 0);
  return NO_ERROR;
}