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

using namespace boost::hana::literals;

#define ND __declspec(noalias)

#define CREATE_PARAM(r, data, idx, type) (type BOOST_PP_CAT(arg, idx))
#define CREATE_ARG(r, data, idx, type) (BOOST_PP_CAT(arg, idx))

#define RETURN_FN BOOST_HANA_STRING("return_fn")
#define CAPTURE_ARGS_MAPS BOOST_HANA_STRING("capture_map")
#define TIMES BOOST_HANA_STRING("cardinality")

constexpr auto DefaultExpectationsMap = boost::hana::make_map(
    boost::hana::make_pair(TIMES, -1_c),
    boost::hana::make_pair(RETURN_FN, -1_c),
    boost::hana::make_pair(CAPTURE_ARGS_MAPS, boost::hana::make_map()));

BOOST_HANA_CONSTEXPR_LAMBDA auto Times = [](auto times_val,
                                            auto expectations_map) {
  static_assert(times_val >= 0,
                "Function call cardinality must be zero or greater!");
  auto tmp = boost::hana::erase_key(expectations_map, TIMES);
  return boost::hana::insert(tmp, boost::hana::make_pair(TIMES, times_val));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto ReturnFn = [](auto ret_fn_val,
                                               auto expectations_map) {
  auto tmp = boost::hana::erase_key(expectations_map, RETURN_FN);
  return boost::hana::insert(tmp,
                             boost::hana::make_pair(RETURN_FN, ret_fn_val));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto Capture = [](auto capture_map,
                                              auto expectations_map) {
  auto tmp = boost::hana::erase_key(expectations_map, CAPTURE_ARGS_MAPS);
  return boost::hana::insert(
      tmp, boost::hana::make_pair(CAPTURE_ARGS_MAPS, capture_map));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto Expect = [](auto func, auto arg0) {
  return boost::hana::partial(func, arg0);
};

BOOST_HANA_CONSTEXPR_LAMBDA auto MakeExpectation = [](auto func) {
  return boost::hana::apply(func, DefaultExpectationsMap);
};

BOOST_HANA_CONSTEXPR_LAMBDA auto AND = boost::hana::infix(
    [](auto fn_x, auto fn_y) { return boost::hana::compose(fn_x, fn_y); });

#define UNPACK_TRANSFORM_TUPLE(func, tuple)                                    \
  BOOST_PP_TUPLE_ENUM(BOOST_PP_SEQ_TO_TUPLE(                                   \
      BOOST_PP_SEQ_FOR_EACH_I(func, DONT_CARE, BOOST_PP_TUPLE_TO_SEQ(tuple))))

#define CREATE_ND_FUNC_NAME(name, type)                                        \
  BOOST_PP_CAT(nd_, BOOST_PP_CAT(name, BOOST_PP_CAT(type, _fn)))

#define LAZY_MOCK_FUNCTION(name, ret_type, args_tuple)                         \
  MOCK_FUNCTION(name, DefaultExpectationsMap, ret_type, args_tuple)

#define MOCK_FUNCTION(name, expectations_map, ret_type, args_tuple)            \
  extern ret_type CREATE_ND_FUNC_NAME(name, ret_type)(void);                   \
  BOOST_HANA_CONSTEXPR_LAMBDA auto name##_ret_fn = []() {                      \
    return CREATE_ND_FUNC_NAME(name, ret_type)();                              \
  };                                                                           \
  ret_type name(UNPACK_TRANSFORM_TUPLE(CREATE_PARAM, args_tuple)) {            \
    return boost::hana::eval_if(                                               \
        boost::hana::at_key(expectations_map, RETURN_FN) == -1_c,              \
        [&]() {                                                                \
          constexpr auto tmp =                                                 \
              boost::hana::erase_key(expectations_map, RETURN_FN);             \
          constexpr auto new_map = boost::hana::insert(                        \
              tmp, boost::hana::make_pair(RETURN_FN, name##_ret_fn));          \
          return boost::hana::apply(                                           \
              skeletal, new_map,                                               \
              boost::hana::make_tuple(                                         \
                  UNPACK_TRANSFORM_TUPLE(CREATE_ARG, args_tuple)));            \
        },                                                                     \
        [&]() {                                                                \
          return boost::hana::apply(                                           \
              skeletal, expectations_map,                                      \
              boost::hana::make_tuple(                                         \
                  UNPACK_TRANSFORM_TUPLE(CREATE_ARG, args_tuple)));            \
        });                                                                    \
  }

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
static BOOST_HANA_CONSTEXPR_LAMBDA auto skeletal = [](auto &&expectations_map,
                                                      auto &&args_tuple) {
  static int timesCounter;
  auto cardinality = boost::hana::at_key(expectations_map, TIMES);
  // cardinality of -1 means don't care
  if (cardinality >= 0_c) {
    timesCounter++;
    sassert(timesCounter <= cardinality);
  }
  auto ret_fn = boost::hana::at_key(expectations_map, RETURN_FN);
  auto capture_map = boost::hana::at_key(expectations_map, CAPTURE_ARGS_MAPS);
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
