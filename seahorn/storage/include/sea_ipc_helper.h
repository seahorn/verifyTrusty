#pragma once

#include <ipc.h>
#include <trusty_ipc.h>

/*
 * Same functionality as dispatch_event but non-static
 */
void sea_dispatch_event(const uevent_t* ev);

/*
 * Initialize a port context by given a channel context handler
 */
struct ipc_port_context* create_port_context(void);