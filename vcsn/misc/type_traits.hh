#pragma once

#include <type_traits>

namespace vcsn
{
  // Wait for C++14.  GCC 4.8 does not yet have std::enable_if_t and
  // the like.
  template <bool B, typename T, typename U>
  using conditional_t = typename std::conditional<B, T, U>::type;

  template <bool Cond, typename T = void>
  using enable_if_t = typename std::enable_if<Cond, T>::type;

  template <typename T, typename U>
  using is_same_t = typename std::is_same<T, U>::type;

  template <typename T>
  using remove_const_t = typename std::remove_const<T>::type;

  template <typename T>
  using remove_cv_t = typename std::remove_cv<T>::type;

  template <typename T>
  using remove_reference_t = typename std::remove_reference<T>::type;

  namespace detail
  {
    // See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf.
#if __GNUC__ < 5 && ! defined __clang__
    // http://stackoverflow.com/a/28967049/1353549
    template <typename...>
    struct voider
    {
      using type = void;
    };
    template <typename...Ts>
    using void_t = typename voider<Ts...>::type;
#else
    template <typename...>
    using void_t = void;
#endif

    // Primary template handles all types not supporting the operation.
    template <typename, template <typename> class, typename = void_t<>>
    struct detect : std::false_type {};

    // Specialization recognizes/validates only types supporting the archetype.
    template <typename T, template <typename> class Op>
    struct detect<T, Op, void_t<Op<T>>> : std::true_type {};
  }
}
