#pragma once

#include <trusty_ipc.h>



ssize_t send_msg(handle_t handle, struct ipc_msg *msg);
int msg_send_called;