/**
 * Stubbed version of trusty IPC
 **/

//#include <trusty_ipc.h>
#include <nondet.h>
#include <seahorn/seahorn.h>
#include <sea_tipc_helper.h>

extern int nd_size_t(void);
extern int msg_send_called;

handle_t port_create(const char* path,
                     uint32_t num_recv_bufs,
                     uint32_t recv_buf_size,
                     uint32_t flags) {
    return nd_port_handle();
}

handle_t connect(const char* path, uint32_t flags) {
    handle_t rc =  nd_chan_handle();
    assume(rc <= 0);
    return rc;
}

handle_t accept(handle_t handle, struct uuid* peer_uuid) {
    return nd_chan_handle();
}

int close(handle_t handle) {
    return nd_int();
}

int set_cookie(handle_t handle, void* cookie) {
    return nd_int();
}

handle_t handle_set_create(void) {
    return nd_chan_handle();
}

int handle_set_ctrl(handle_t handle, uint32_t cmd, struct uevent* evt) {
    return nd_int();
}

int wait(handle_t handle, struct uevent* event, uint32_t timeout_msecs) {
    return nd_int();
}

int wait_any(struct uevent* event, uint32_t timeout_msecs) {
    return nd_int();
}

int get_msg(handle_t handle, struct ipc_msg_info* msg_info) {
    return nd_int();
}

ssize_t read_msg(handle_t handle,
                 uint32_t msg_id,
                 uint32_t offset,
                 struct ipc_msg* msg) {
    return nd_int();
}

int put_msg(handle_t handle, uint32_t msg_id) {
    return nd_int();
}

ssize_t send_msg(handle_t handle, struct ipc_msg* msg) {
    msg_send_called = 1;
    return nd_size_t();
}