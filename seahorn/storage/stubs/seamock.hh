#ifndef SEAMOCK_H_
#define SEAMOCK_H_

#include <nondet.h>
#include <seahorn/seahorn.h>

#include <boost/hana.hpp>
#include <boost/hana/assert.hpp>
#include <boost/preprocessor.hpp>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ND __declspec(noalias)

#define CREATE_PARAM(r, data, idx, type) (type BOOST_PP_CAT(arg, idx))
#define CREATE_ARG(r, data, idx, type) (BOOST_PP_CAT(arg, idx))

#define UNPACK_TRANSFORM_TUPLE(func, tuple)                                    \
  BOOST_PP_TUPLE_ENUM(BOOST_PP_SEQ_TO_TUPLE(                                   \
      BOOST_PP_SEQ_FOR_EACH_I(func, DONT_CARE, BOOST_PP_TUPLE_TO_SEQ(tuple))))

#define MOCK_FUNCTION(name, ret_fn, capture_map, args_tuple)                   \
  BOOST_HANA_CONSTEXPR_LAMBDA auto name##_partially_applied_fn =               \
      [](auto ret_fn_l, auto capture_map_l) {                                  \
        auto func = boost::hana::capture(ret_fn_l, capture_map_l)(skeletal);   \
        return func;                                                           \
      };                                                                       \
  decltype(ret_fn()) name(UNPACK_TRANSFORM_TUPLE(CREATE_PARAM, args_tuple)) {  \
    return boost::hana::apply(skeletal, ret_fn, capture_map,                   \
                              boost::hana::make_tuple(UNPACK_TRANSFORM_TUPLE(  \
                                  CREATE_ARG, args_tuple)));                   \
  }

#define CREATE_ND_FUNC_NAME(name, type)                                        \
  BOOST_PP_CAT(nd_, BOOST_PP_CAT(name, BOOST_PP_CAT(type, _fn)))

#define MOCK_FUNCTION_RETURN_ANY_NO_CAPTURE(name, ret_type, args_tuple)        \
  extern ret_type CREATE_ND_FUNC_NAME(name, ret_type)(void);                   \
  BOOST_HANA_CONSTEXPR_LAMBDA auto name##_ret_fn = []() {                      \
    return CREATE_ND_FUNC_NAME(name, ret_type)();                              \
  };                                                                           \
  MOCK_FUNCTION(name, name##_ret_fn, boost::hana::make_map(), args_tuple)

extern "C" {
ND int nd_trusty_ipc_err(void);
ND size_t nd_msg_len(void);
ND size_t nd_size(void);
ND uint32_t nd_msg_id(void);
ND void memhavoc(void *ptr, size_t size);
}
// ---------------------------------------------
// Generic mock fn
// ---------------------------------------------
static BOOST_HANA_CONSTEXPR_LAMBDA auto skeletal = [](auto &&ret_fn,
                                                      auto &&capture_map,
                                                      auto &&args_tuple) {
  // NOTE: INVARIANT: return fn should be callable
  boost::hana::is_valid([&ret_fn]() -> decltype(ret_fn()) { return 0; });
  // NOTE: (arg0, arg1, ..._N) -> (0, 1, ..._N)
  auto args_range = boost::hana::make_range(boost::hana::size_c<0>,
                                            boost::hana::size(args_tuple));
  // BOOST_HANA_CONSTANT_ASSERT(boost::hana::size(args_tuple) ==
  //                            boost::hana::size_c<2>);
  // BOOST_HANA_CONSTANT_ASSERT(boost::hana::size(args_range) ==
  //                            boost::hana::size_c<2>);
  // NOTE: (0, 1, ..._N), (arg0, arg1, ..._N) --> ((0, arg0), (1, arg1),
  // ..._N)
  auto indexed_args_pairs =
      boost::hana::zip(boost::hana::to_tuple(args_range), args_tuple);
  // NOTE: e.g., ((1, P1), (3, P3)) --> (1, 3)
  auto capture_params_indices = boost::hana::keys(capture_map);
  // NOTE:  _ --> ((1, arg1), (3, arg3))
  auto filtered_args = boost::hana::filter(indexed_args_pairs, [&](auto pair) {
    auto idx = boost::hana::at(pair, boost::hana::size_c<0>);
    return boost::hana::contains(capture_params_indices, idx);
  });
  // BOOST_HANA_CONSTANT_ASSERT(boost::hana::size(filtered_args) ==
  //                            boost::hana::size_c<1>);
  // NOTE: ((1, arg1), (3, arg3)) --> (arg1, arg3)
  auto args = boost::hana::transform(filtered_args, [&](auto pair) {
    return boost::hana::at(pair, boost::hana::size_c<1>);
  });
  // NOTE: ((1, P1), (3, P3)) --> (P1, P3)
  auto capture_params_values = boost::hana::values(capture_map);
  // NOTE: (P1, P3), (arg1, arg3)  --> ((P1, arg1), (P3, arg3))
  auto param_arg_pairs = boost::hana::zip(capture_params_values, args);
  // BOOST_HANA_CONSTANT_ASSERT(boost::hana::size(param_arg_pairs) ==
  //                            boost::hana::size_c<1>);
  // NOTE: call functions P1(arg1), P3(arg3) for side-effects
  boost::hana::for_each(param_arg_pairs, [&](auto pair) {
    auto param = boost::hana::at(pair, boost::hana::size_c<0>);
    auto arg = boost::hana::at(pair, boost::hana::size_c<1>);

    boost::hana::apply(param, arg);
  });
  return ret_fn();
};

#endif // SEAMOCK_H_
