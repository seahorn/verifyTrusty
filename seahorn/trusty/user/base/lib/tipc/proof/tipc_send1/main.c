#include <seahorn/seahorn.h>
#include <lib/tipc/tipc.h>
#include <trusty_ipc.h>
#include <sea_tipc_helper.h>

extern handle_t nd_handle(void);
extern int nd_size_t(void);
extern void *sea_malloc_havoc(size_t sz);

/** Test harness for tpic_send1 */
int main(void) {

    
    /* assumptions */
    state_init();
    handle_t handle = nd_handle();

    size_t buf_len = nd_size_t();
    assume(buf_len > 0);

    void *buf = sea_malloc_havoc(buf_len);
    
//   sea_tracking_on();
//   sea_reset_modified((char *)buf);

   /* send single buf message  */
   int rc = tipc_send1(handle, buf, buf_len);

 //  sassert(!sea_is_modified((char *)buf));

   /* testing the message is only sent once */
   int msg_sent = get_msg_sent_times();
   sassert(msg_sent == 1);
   state_reset();
   return 0;
}