#pragma once

#include <ipc.h>
// #include <trusty_ipc.h>

/*
 * Same functionality as dispatch_event but non-static
 */
void sea_dispatch_event(const uevent_t *ev);

/*
 * Initialize a port context by given a channel context handler
 */
struct ipc_port_context *create_port_context(void);

struct ipc_channel_context *
sea_sync_channel_connect(struct ipc_port_context *parent_ctx,
                         const uuid_t *peer_uuid, handle_t chan_handle);
