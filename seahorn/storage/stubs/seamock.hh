#ifndef SEAMOCK_H_
#define SEAMOCK_H_

#include <boost/hana/fwd/intersection.hpp>
#include <boost/hana/fwd/tuple.hpp>
#include <boost/hana/string.hpp>
#include <nondet.h>
#include <seahorn/seahorn.h>

#include <array>
#include <boost/hana.hpp>
#include <boost/hana/assert.hpp>
#include <boost/preprocessor.hpp>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

using namespace boost::hana::literals;

#define ND __declspec(noalias)

extern "C" {
extern void sea_printf(const char *format, ...);
}

#define CREATE_PARAM(r, data, idx, type) (type BOOST_PP_CAT(arg, idx))
#define CREATE_ARG(r, data, idx, type) (BOOST_PP_CAT(arg, idx))
// name -> BOOST_HANA_STRING("name")
#define HANA_STRINGIZE_OP(s, _data, elem)                                      \
  (BOOST_HANA_STRING(BOOST_PP_STRINGIZE(elem)))

// Expectation map keys
#define CALL_FN_NAME BOOST_HANA_STRING("call_fn_name")
#define RETURN_FN BOOST_HANA_STRING("return_fn")
#define CAPTURE_ARGS_MAPS BOOST_HANA_STRING("capture_map")
#define TIMES BOOST_HANA_STRING("cardinality")
#define AFTER BOOST_HANA_STRING("predecessors")

#define SEQ_COUNTER_MAXVAL 10
static size_t g_sequence_counter;

namespace hana = boost::hana;

namespace seamock {
namespace util {

// auto SeqTuple = hana::make_tuple(-1_c, -1_c, -1_c);
// const char *SeqArray[3] = {"UNDEF", "UNDEF", "UNDEF"};
static std::array<const char *, 5> SeqArray{"UNDEF", "UNDEF", "UNDEF", "UNDEF",
                                            "UNDEF"};

} // namespace util
} // namespace seamock

static constexpr auto DefaultExpectationsMap = hana::make_map(
    hana::make_pair(CALL_FN_NAME, -1_c), hana::make_pair(TIMES, -1_c),
    hana::make_pair(RETURN_FN, -1_c),
    hana::make_pair(CAPTURE_ARGS_MAPS, hana::make_map()),
    hana::make_pair(AFTER, hana::make_tuple()));

BOOST_HANA_CONSTEXPR_LAMBDA auto Times = [](auto times_val,
                                            auto expectations_map) {
  static_assert(times_val >= 0,
                "Function call cardinality must be zero or greater!");
  auto tmp = hana::erase_key(expectations_map, TIMES);
  return hana::insert(tmp, hana::make_pair(TIMES, times_val));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto ReturnFn = [](auto ret_fn_val,
                                               auto expectations_map) {
  auto tmp = hana::erase_key(expectations_map, RETURN_FN);
  return hana::insert(tmp, hana::make_pair(RETURN_FN, ret_fn_val));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto Capture = [](auto capture_map,
                                              auto expectations_map) {
  auto tmp = hana::erase_key(expectations_map, CAPTURE_ARGS_MAPS);
  return hana::insert(tmp, hana::make_pair(CAPTURE_ARGS_MAPS, capture_map));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto After = [](auto predecessor_tup,
                                            auto expectations_map) {
  auto tmp = hana::erase_key(expectations_map, AFTER);
  return hana::insert(tmp, hana::make_pair(AFTER, predecessor_tup));
};

BOOST_HANA_CONSTEXPR_LAMBDA auto Expect = [](auto func, auto arg0) {
  return hana::partial(func, arg0);
};

BOOST_HANA_CONSTEXPR_LAMBDA auto MakeExpectation = [](auto func) {
  return hana::apply(func, DefaultExpectationsMap);
};

BOOST_HANA_CONSTEXPR_LAMBDA auto AND =
    hana::infix([](auto fn_x, auto fn_y) { return hana::compose(fn_x, fn_y); });

#define UNPACK_TRANSFORM_TUPLE(func, tuple)                                    \
  BOOST_PP_TUPLE_ENUM(BOOST_PP_SEQ_TO_TUPLE(                                   \
      BOOST_PP_SEQ_FOR_EACH_I(func, DONT_CARE, BOOST_PP_TUPLE_TO_SEQ(tuple))))

// set is actually a hana tuple but we don't account for order so
// tell the user they are making a set
#define MAKE_PRED_FN_SET(fn_names...)                                          \
  hana::make_tuple(BOOST_PP_TUPLE_ENUM(BOOST_PP_SEQ_TO_TUPLE(                  \
      BOOST_PP_SEQ_TRANSFORM(HANA_STRINGIZE_OP, 0 /* don't care */,            \
                             BOOST_PP_VARIADIC_TO_SEQ(fn_names)))))

#define CREATE_ND_FUNC_NAME(name, type)                                        \
  BOOST_PP_CAT(nd_, BOOST_PP_CAT(name, BOOST_PP_CAT(type, _fn)))

#define LAZY_MOCK_FUNCTION(name, ret_type, args_tuple)                         \
  MOCK_FUNCTION(name, DefaultExpectationsMap, ret_type, args_tuple)

#define MOCK_FUNCTION(name, expectations_map, ret_type, args_tuple)            \
  extern ret_type CREATE_ND_FUNC_NAME(name, ret_type)(void);                   \
  BOOST_HANA_CONSTEXPR_LAMBDA auto name##_ret_fn = []() {                      \
    return CREATE_ND_FUNC_NAME(name, ret_type)();                              \
  };                                                                           \
  constexpr auto expectations_map_w_name_##name =                              \
      hana::insert(hana::erase_key(expectations_map, CALL_FN_NAME),            \
                   hana::make_pair(CALL_FN_NAME, BOOST_HANA_STRING(#name)));   \
  static_assert(hana::at_key(expectations_map_w_name_##name, CALL_FN_NAME) !=  \
                -1_c);                                                         \
  ret_type name(UNPACK_TRANSFORM_TUPLE(CREATE_PARAM, args_tuple)) {            \
    return hana::eval_if(                                                      \
        hana::at_key(expectations_map, RETURN_FN) == -1_c,                     \
        [&]() {                                                                \
          constexpr auto tmp =                                                 \
              hana::erase_key(expectations_map_w_name_##name, RETURN_FN);      \
          constexpr auto new_map =                                             \
              hana::insert(tmp, hana::make_pair(RETURN_FN, name##_ret_fn));    \
          auto partfn = hana::partial(skeletal, new_map);                      \
          return hana::apply(partfn, hana::make_tuple(UNPACK_TRANSFORM_TUPLE(  \
                                         CREATE_ARG, args_tuple)));            \
        },                                                                     \
        [&]() {                                                                \
          auto partfn =                                                        \
              hana::partial(skeletal, expectations_map_w_name_##name);         \
          return hana::apply(partfn, hana::make_tuple(UNPACK_TRANSFORM_TUPLE(  \
                                         CREATE_ARG, args_tuple)));            \
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
  auto cardinality = hana::at_key(expectations_map, TIMES);
  auto fnName = hana::at_key(expectations_map, CALL_FN_NAME);
  static_assert(fnName != -1_c);
  // NOTE: record call in Sequence
  seamock::util::SeqArray[g_sequence_counter] = fnName.c_str();
  // seamock::util::SetTupleAtIdx(seamock::util::SeqTuple, g_sequence_counter,
  //                              fnName);
  // NOTE: update global sequence counter
  g_sequence_counter++;
  // cardinality of -1 means don't care
  if (cardinality >= 0_c) {
    timesCounter++;
    sassert(timesCounter <= cardinality);
  }
  // NOTE: check that after constraint is maintained
  auto pred_set = hana::at_key(expectations_map, AFTER);
  auto pred_found_tup = hana::transform(pred_set, [&](auto elem) {
    // TODO: search till g_sequence_counter - 1 instead of std::end
    auto it = std::find(std::begin(seamock::util::SeqArray),
                        std::end(seamock::util::SeqArray), elem.c_str());
    return it != std::end(seamock::util::SeqArray) &&
           std::distance(std::begin(seamock::util::SeqArray), it) <
               g_sequence_counter - 1;
  });
  if (!hana::fold(pred_found_tup, true,
                  [&](auto acc, bool element) { return acc && element; })) {
    sea_printf("Predecessor (After) match failed!\n");
    sassert(0);
  };
  auto ret_fn = hana::at_key(expectations_map, RETURN_FN);
  auto capture_map = hana::at_key(expectations_map, CAPTURE_ARGS_MAPS);
  // NOTE: INVARIANT: return fn should be callable
  hana::is_valid([&ret_fn]() -> decltype(ret_fn()) { return 0; });
  // NOTE: (arg0, arg1, ..._N) -> (0, 1, ..._N)
  auto args_range = hana::make_range(hana::size_c<0>, hana::size(args_tuple));
  // BOOST_HANA_CONSTANT_ASSERT(hana::size(args_tuple) ==
  //                            hana::size_c<2>);
  // BOOST_HANA_CONSTANT_ASSERT(hana::size(args_range) ==
  //                            hana::size_c<2>);
  // NOTE: (0, 1, ..._N), (arg0, arg1, ..._N) --> ((0, arg0), (1, arg1),
  // ..._N)
  auto indexed_args_pairs = hana::zip(hana::to_tuple(args_range), args_tuple);
  // NOTE: e.g., ((1, P1), (3, P3)) --> (1, 3)
  auto capture_params_indices = hana::keys(capture_map);
  // NOTE:  _ --> ((1, arg1), (3, arg3))
  auto filtered_args = hana::filter(indexed_args_pairs, [&](auto pair) {
    auto idx = hana::at(pair, hana::size_c<0>);
    return hana::contains(capture_params_indices, idx);
  });
  // BOOST_HANA_CONSTANT_ASSERT(hana::size(filtered_args) ==
  //                            hana::size_c<1>);
  // NOTE: ((1, arg1), (3, arg3)) --> (arg1, arg3)
  auto args = hana::transform(filtered_args, [&](auto pair) {
    return hana::at(pair, hana::size_c<1>);
  });
  // NOTE: ((1, P1), (3, P3)) --> (P1, P3)
  auto capture_params_values = hana::values(capture_map);
  // NOTE: (P1, P3), (arg1, arg3)  --> ((P1, arg1), (P3, arg3))
  auto param_arg_pairs = hana::zip(capture_params_values, args);
  // BOOST_HANA_CONSTANT_ASSERT(hana::size(param_arg_pairs) ==
  //                            hana::size_c<1>);
  // NOTE: call functions P1(arg1), P3(arg3) for side-effects
  hana::for_each(param_arg_pairs, [&](auto pair) {
    auto param = hana::at(pair, hana::size_c<0>);
    auto arg = hana::at(pair, hana::size_c<1>);

    hana::apply(param, arg);
  });
  return ret_fn();
};

#endif // SEAMOCK_H_
