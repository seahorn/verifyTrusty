#ifndef _SEA_TRUSTY_SYSCALLS_H_
#define _SEA_TRUSTY_SYSCALLS_H_
#include <trusty_ipc.h>

handle_t _trusty_port_create(const char *path, uint32_t num_recv_bufs,
                             uint32_t recv_buf_size, uint32_t flags);

handle_t _trusty_connect(const char *path, uint32_t flags);

handle_t _trusty_accept(handle_t handle, struct uuid *peer_uuid);

int _trusty_close(handle_t handle);

int _trusty_set_cookie(handle_t handle, void *cookie);

handle_t _trusty_handle_set_create(void);

int _trusty_handle_set_ctrl(handle_t handle, uint32_t cmd, struct uevent *evt);

int _trusty_wait(handle_t handle, struct uevent *event, uint32_t timeout_msecs);

int _trusty_wait_any(struct uevent *event, uint32_t timeout_msecs);

int _trusty_get_msg(handle_t handle, struct ipc_msg_info *msg_info);

ssize_t _trusty_read_msg(handle_t handle, uint32_t msg_id, uint32_t offset,
                         struct ipc_msg *msg);

int _trusty_put_msg(handle_t handle, uint32_t msg_id);

ssize_t _trusty_send_msg(handle_t handle, struct ipc_msg *msg);

handle_t _trusty_dup(handle_t handle);

#endif
