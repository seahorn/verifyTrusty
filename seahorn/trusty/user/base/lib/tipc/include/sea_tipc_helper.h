#pragma once

#include <trusty_ipc.h>



ssize_t send_msg(handle_t handle, struct ipc_msg *msg);

typedef struct State{
    int msg_send_called;
    int msg_recv_called;
} state;

state g_state;

void state_init(void);
void msg_sent(void);
void msg_received(void);
void state_reset(void);
int get_msg_sent_times(void);