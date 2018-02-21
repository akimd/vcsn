#pragma once

#include <iostream>
#include <tuple>

#include <vcsn/misc/functional.hh>
#include <vcsn/misc/type_traits.hh> // bool_constant

// These definitions come in handy every time we define variadic tuples.
namespace vcsn::detail
{

  /*-----------------.
  | index_sequence.  |
  `-----------------*/

  // See "Pretty-print std::tuple"
  // <http://stackoverflow.com/questions/6245735>.

  // See O(log N) implementation of integer sequence
  // <http://stackoverflow.com/questions/17424477>

  /// A static list of size_t.
  template <std::size_t...>
  struct index_sequence
  {
    using type = index_sequence;
  };

  /*-----------------------.
  | make_index_sequence.   |
  `-----------------------*/

  /// Concat two sequences of size_t, adding the size of the first.
  /// E.g., `<0, 1, 2>, <0, 1, 2, 3> -> <0, 1, 2, 3, 4, 5, 6>`.
  template <typename S1, typename S2>
  struct concat;

  template <std::size_t... I1, std::size_t... I2>
  struct concat<index_sequence<I1...>, index_sequence<I2...>>
    : index_sequence<I1..., (sizeof...(I1)+I2)...>
  {};

  template <typename S1, typename S2>
  using concat_t = typename concat<S1, S2>::type;

  /// Build the static sequence of size_t [0, N[.
  template <std::size_t N>
  struct make_index_sequence;
  template <std::size_t N>
  using make_index_sequence_t = typename make_index_sequence<N>::type;

  template <std::size_t N>
  struct make_index_sequence
    : concat_t<make_index_sequence_t<N/2>,
               make_index_sequence_t<N - N/2>>
  {};

  template <> struct make_index_sequence<0> : index_sequence<>{};
  template <> struct make_index_sequence<1> : index_sequence<0>{};

  template <std::size_t off, typename S2>
  struct int_range;

  template <std::size_t off, std::size_t... I>
  struct int_range<off, index_sequence<I...>>
    : index_sequence<I + off...>
  {};



  /*--------------------.
  | make_index_range.   |
  `--------------------*/

  /// A static range.
  ///
  /// \tparam S  starting point of the sequence.
  /// \tparam L  length of the sequence.
  template <std::size_t S, std::size_t L>
  struct make_index_range_impl
    : int_range<S, make_index_sequence_t<L>>
  {};


  template <std::size_t S>
  struct make_index_range_impl<S, 0> : index_sequence<>{};
  template <std::size_t S>
  struct make_index_range_impl<S, -1U> : index_sequence<>{};

  /// A static range.
  ///
  /// \tparam S  starting point of the sequence.
  /// \tparam E  end of the sequence, excluded.
  template <std::size_t S, std::size_t E>
  struct make_index_range
    : make_index_range_impl<S, E - S>
  {};

  template <std::size_t S, std::size_t E>
  using make_index_range_t = typename make_index_range<S, E>::type;


  /*-------------------.
  | concat_sequence.   |
  `-------------------*/

  /// Concatenate two static sequences of size_t.
  template <typename S1, typename S2>
  struct concat_index_sequence;

  template <std::size_t... I1, std::size_t... I2>
  struct concat_index_sequence<index_sequence<I1...>, index_sequence<I2...>>
    : index_sequence<I1..., I2...>{};

  template <typename S1, typename S2>
  using concat_sequence = typename concat_index_sequence<S1, S2>::type;

  // There is a bug in clang making this one useless...
  // The index sequence generated is always <0>
  // Bug report:
  // http://llvm.org/bugs/show_bug.cgi?id=14858
  //template <typename... T>
  //using index_sequence_for = make_index_sequence<sizeof...(T)>;


  /*-----------------------.
  | sequence_difference.   |
  `-----------------------*/

  /// The list containing all the elements of I1 (contiguous
  /// sequence from 0 to N) not present in I2 (arbitrary sequence,
  /// sorted).
  template <typename S1, typename S2>
  struct index_sequence_difference;

  template <typename S1, typename S2>
  using sequence_difference
    = typename index_sequence_difference<typename S1::type,
                                         typename S2::type>::type;

  template <std::size_t I1_1, std::size_t... I1, std::size_t... I2>
  struct index_sequence_difference<index_sequence<I1_1, I1...>,
                                   index_sequence<I1_1, I2...>>
  {
    using type = sequence_difference<index_sequence<I1...>,
                                     index_sequence<I2...>>;
  };

  template <std::size_t I1_1, std::size_t I2_1,
            std::size_t... I1, std::size_t... I2>
  struct index_sequence_difference<index_sequence<I1_1, I1...>,
                                   index_sequence<I2_1, I2...>>
  {
    using type =
      concat_sequence<index_sequence<I1_1>,
                      sequence_difference<index_sequence<I1...>,
                                          index_sequence<I2_1, I2...>>>;
  };

  template <std::size_t I1_1, std::size_t... I1>
  struct index_sequence_difference<index_sequence<I1_1, I1...>,
                                   index_sequence<>>
  {
    using type =
      concat_sequence<index_sequence<I1_1>,
                      sequence_difference<index_sequence<I1...>,
                                          index_sequence<>>>;
  };

  template <>
  struct index_sequence_difference<index_sequence<>, index_sequence<>>
  {
    using type = typename index_sequence<>::type;
  };


  /*--------------------.
  | punched_sequence.   |
  `--------------------*/

  /// An index sequence with a gap.
  template <std::size_t N, std::size_t Gap>
  using punched_sequence
    = concat_sequence<make_index_range_t<0, Gap>,
                      make_index_range_t<Gap + 1, N>>;


  /*--------.
  | for_.   |
  `--------*/


  /// Run function \a f on each member of \a ts.
  template <typename Fun, typename... Ts>
  void
  for_(const std::tuple<Ts...>& ts, Fun f)
  {
    for_(f, ts, make_index_sequence<sizeof...(Ts)>());
  }

  template <typename Fun, typename... Ts, size_t... I>
  void
  for_(Fun f,
       const std::tuple<Ts...>& ts,
       index_sequence<I...>)
  {
    using swallow = int[];
    (void) swallow{ (f(std::get<I>(ts)), 0)... };
  }


  /// Map a function on a tuple, return tuple of the results.
  template <typename Fun, typename... Ts>
  auto
  map(const std::tuple<Ts...>& ts, Fun f)
    -> decltype(map_tuple_(f, ts, make_index_sequence<sizeof...(Ts)>()))
  {
    return map_impl_(f, ts, make_index_sequence<sizeof...(Ts)>());
  }

  template <typename Fun, typename... Ts, size_t... I>
  auto
  map_impl_(Fun f,
            const std::tuple<Ts...>& ts,
            index_sequence<I...>)
  {
    return std::make_tuple(f(std::get<I>(ts))...);
  }


  /// Unpack a tuple, and pass its content as argument to a funtion.
  template <typename Fun, typename... Args>
  auto
  apply(Fun f, const std::tuple<Args...>& args)
    // Return type needed by G++ 5.
    -> decltype(apply_impl_(f, args, make_index_sequence<sizeof...(Args)>()))
  {
    return apply_impl_(f, args, make_index_sequence<sizeof...(Args)>());
  }

  template <typename Fun, typename... Args, size_t... I>
  auto
  apply_impl_(Fun f,
              const std::tuple<Args...>& args,
              index_sequence<I...>)
    // Return type needed by G++ 5.
    -> decltype(f(std::get<I>(args)...))
  {
    return f(std::get<I>(args)...);
  }


  /// Apply a tuple of functions on a tuple of arguments, return the
  /// tuple of results.
  template <typename... Funs, typename... Args>
  auto
  apply(const std::tuple<Funs...>& funs, const std::tuple<Args...>& args)
  {
    static_assert(sizeof...(Funs) == sizeof...(Args),
                  "tuples of functions and arguments of different sizes");
    return apply_impl_(funs, args, make_index_sequence<sizeof...(Funs)>());
  }

  /// Apply a tuple of functions on a tuple of arguments, return the
  /// tuple of results.
  template <typename... Funs, typename... Args, size_t... I>
  auto
  apply_impl_(const std::tuple<Funs...>& funs,
              const std::tuple<Args...>& args,
              index_sequence<I...>)
  {
    return std::make_tuple(std::get<I>(funs)(std::get<I>(args))...);
  }


  /// Apply a tuple of member functions on a tuple of arguments,
  /// return the tuple of results.
  template <typename Fun, typename... Objs, typename... Args>
  auto
  apply(Fun fun,
        const std::tuple<Objs...>& objs, const std::tuple<Args...>& args)
  {
    static_assert(sizeof...(Objs) == sizeof...(Args),
                  "tuples of objects and arguments of different sizes");
    return apply_impl_(fun, objs, args,
                       make_index_sequence<sizeof...(Objs)>());
  }

  template <typename Fun, typename... Objs, typename... Args, size_t... I>
  auto
  apply_impl_(Fun fun,
              const std::tuple<Objs...>& objs,
              const std::tuple<Args...>& args,
              index_sequence<I...>)
  {
    return std::make_tuple(fun(std::get<I>(objs), std::get<I>(args))...);
  }


  /*--------------------------------------------.
  | Variadic Cartesian product of containers.   |
  `--------------------------------------------*/

  /// Variadic Cartesian product of containers.
  ///
  /// Based on http://stackoverflow.com/questions/13813007/
  ///
  /// Beware of name conflicts with vcsn/misc/cross.
  template <typename Fun>
  void
  cross(Fun f)
  {
    f();
  }

  template <typename Fun,
            typename Cont, typename... Conts>
  void
  cross(Fun f,
        const Cont& head, const Conts&... tails)
  {
    for (const typename Cont::value_type& h: head)
      cross([&](const typename Conts::value_type&... tails)
            { f(h, tails...); },
            tails...);
  }

  template <typename Fun, typename... Ts, size_t... I>
  void
  cross_tuple_(Fun f,
               const std::tuple<Ts...>& ts,
               index_sequence<I...>)
  {
    cross(f, std::get<I>(ts)...);
  }

  template <typename Fun, typename... Ts>
  void
  cross_tuple(Fun f,
              const std::tuple<Ts...>& ts)
  {
    cross_tuple_(f, ts, make_index_sequence<sizeof...(Ts)>());
  }



  /*----------------.
  | reverse_tuple.  |
  `----------------*/

  template <typename... Ts>
  auto
  reverse_tuple(const std::tuple<Ts...>& t)
    -> decltype(reverse_tuple(t, make_index_sequence<sizeof...(Ts)>()))
  {
    return reverse_tuple(t, make_index_sequence<sizeof...(Ts)>());
  }

  template <typename... Ts, std::size_t... I>
  auto
  reverse_tuple(const std::tuple<Ts...>& t, index_sequence<I...>)
    -> decltype(std::make_tuple(std::get<sizeof...(Ts) - 1 - I>(t)...))
  {
    return std::make_tuple(std::get<sizeof...(Ts) - 1 - I>(t)...);
  }

  /// Same as make_tuple, unless the evaluation of arguments if
  /// right-to-left, in which case reverse the result.
#if VCSN_HAVE_CORRECT_LIST_INITIALIZER_ORDER
  template <typename... Ts>
  auto
  make_gcc_tuple(Ts&&... ts)
    -> decltype(std::make_tuple(std::forward<Ts>(ts)...))
  {
    return std::make_tuple(std::forward<Ts>(ts)...);
  }
#else
  template <typename... Ts>
  auto
  make_gcc_tuple(Ts&&... ts)
    -> decltype(reverse_tuple(std::make_tuple(std::forward<Ts>(ts)...)))
  {
    return reverse_tuple(std::make_tuple(std::forward<Ts>(ts)...));
  }
#endif


  /*------------------------.
  | print(tuple, ostream).  |
  `------------------------*/

  template <typename Tuple, std::size_t N>
  struct tuple_printer
  {
    static void print(const Tuple& t, std::ostream& o)
    {
      tuple_printer<Tuple, N-1>::print(t, o);
      o << ", " << std::get<N-1>(t);
    }
  };

  template <typename Tuple>
  struct tuple_printer<Tuple, 1>
  {
    static void print(const Tuple& t, std::ostream& o)
    {
      o << std::get<0>(t);
    }
  };

  template <typename... Args>
  std::ostream& print(const std::tuple<Args...>& args, std::ostream& o)
  {
    o << '(';
    tuple_printer<decltype(args), sizeof...(Args)>::print(args, o);
    return o << ')';
  }

  /// Whether some of the `values` evaluate as true.
  ///
  /// Made a functor to be easily composable with unpack.
  struct any
  {
    template <typename... Bool>
    bool operator()(Bool&&... values)
    {
      return (values || ...);
    }
  };
}

namespace std
{

  /*-------------------------.
  | std::hash(tuple<T...>).  |
  `-------------------------*/

  template <typename... Elements>
  struct hash<std::tuple<Elements...>>
  {
    using value_t = std::tuple<Elements...>;
    using indices_t = vcsn::detail::make_index_sequence<sizeof...(Elements)>;

    std::size_t operator()(const value_t& v) const
    {
      return hash_(v, indices_t{});
    }

  private:
    template <std::size_t... I>
    static std::size_t
    hash_(const value_t& v, vcsn::detail::index_sequence<I...>)
    {
      std::size_t res = 0;
      using swallow = int[];
      (void) swallow
        {
          (vcsn::hash_combine(res, std::get<I>(v)), 0)...
        };
      return res;
    }
  };
}
