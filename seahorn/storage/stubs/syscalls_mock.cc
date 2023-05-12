#include <boost/hana/functional/compose.hpp>
#include <lk/compiler.h>
#include <sys/uio.h>
#include <uapi/err.h> // trusty errors definitions
#include <uapi/trusty_uuid.h>

#include "seahorn/seahorn.h"
#include <nondet.h>

#include <boost/hana.hpp>
#include <seamock.hh>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define INVALID_IPC_MSG_ID 0
#define ND __declspec(noalias)

extern "C" {
ND int nd_trusty_ipc_err(void);
ND size_t nd_msg_len(void);
ND size_t nd_size(void);
ND uint32_t nd_msg_id(void);
ND void memhavoc(void *ptr, size_t size);
}

// ---------------------------------------------
// Begin of mock functions arg
// ---------------------------------------------

static ssize_t msg_size;

BOOST_HANA_CONSTEXPR_LAMBDA auto ret_fn_get_msg = []() { return nd_int(); };

BOOST_HANA_CONSTEXPR_LAMBDA auto set_pointer_fn_get_msg =
    [](ipc_msg_info_t *msg_info) {
      msg_info->id = nd_msg_id();
      msg_info->len = nd_msg_len();
      msg_size = msg_info->len;
      assume(msg_info->len << (sizeof(size_t) * 8 - 3) == 0);
    };

constexpr auto get_msg_capture_map = boost::hana::make_map(
    boost::hana::make_pair(boost::hana::size_c<1>, set_pointer_fn_get_msg));

BOOST_HANA_CONSTEXPR_LAMBDA auto ret_fn_read_msg = []() {
  return (ssize_t)msg_size;
};

BOOST_HANA_CONSTEXPR_LAMBDA auto set_pointer_fn_read_msg = [](ipc_msg_t *msg) {
  sassert(msg->num_iov == 1);
  char *blob = (char *)malloc(msg_size);
  memhavoc(blob, msg_size);
  sassert(msg->iov);
  sassert(blob);
  sassert(msg->iov[0].iov_base);
  memcpy(msg->iov[0].iov_base, blob, msg_size);
  sea_reset_modified((char *)(msg->iov[0].iov_base));
};

constexpr auto read_msg_capture_map = boost::hana::make_map(
    boost::hana::make_pair(boost::hana::size_c<3>, set_pointer_fn_read_msg));

// ---------------------------------------------
// Emd of mock functions arg
// ---------------------------------------------
extern "C" {

// TODO: rename Times to AtMaxTimes
constexpr auto get_msg_expectations = MakeExpectation(
    Expect(Times, 1_c) ^ AND ^ Expect(ReturnFn, ret_fn_get_msg) ^ AND ^
    Expect(Capture, get_msg_capture_map));
MOCK_FUNCTION(get_msg, get_msg_expectations, int, (handle_t, ipc_msg_info_t *))

constexpr auto read_msg_expectations =
    boost::hana::compose(boost::hana::partial(ReturnFn, ret_fn_read_msg),
                         boost::hana::partial(Capture, read_msg_capture_map))(
        DefaultExpectationsMap);
MOCK_FUNCTION(read_msg, read_msg_expectations, ssize_t,
              (handle_t, uint32_t, uint32_t, ipc_msg_t *))

LAZY_MOCK_FUNCTION(put_msg, int, (handle_t, uint32_t))
}
