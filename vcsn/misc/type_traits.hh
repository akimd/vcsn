#pragma once

#include <type_traits>

namespace vcsn
{
  /*----------.
  | C++ 17.   |
  `----------*/

  template <bool B>
  using bool_constant = std::integral_constant<bool, B>;

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
