#include <seahorn/seahorn.h>
#include <lib/tipc/tipc.h>
#include <trusty_ipc.h>
#include <stdlib.h>

extern handle_t nd_handle(void);
extern int nd_size_t(void);
//extern void* can_fail_malloc(int);
extern int msg_send_called;

/** Test harness for tpic_send1 */
int main(void) {

 // msg_is_send_called = 0;

  /* assumptions */
 msg_send_called = 0;
 handle_t handle = nd_handle();

 size_t buf_len = nd_size_t();
 //void *buf = can_fail_malloc(buf_len);
 void *buf = malloc(buf_len);

 //sea_tracking_on();

 /* send single buf message  */
 int rc = tipc_send1(handle, buf, buf_len);

 // sassert(msg_send_called == 1);

  //sassert(!sea_is_modified(buf));
  sassert(msg_send_called == 1);

  return 0;
}