#pragma once

#include <iostream>
#include <tuple>

#include <vcsn/misc/functional.hh>
#include <vcsn/misc/type_traits.hh> // bool_constant

namespace vcsn::detail
{


  /*--------------------.
  | make_index_range.   |
  `--------------------*/

  template <std::size_t Off, typename S2>
  struct int_range;

  template <std::size_t Off, std::size_t... Is>
  struct int_range<Off, std::index_sequence<Is...>>
  {
    using type = std::index_sequence<Is + Off...>;
  };

  /// A static range.
  ///
  /// \tparam S  starting point of the sequence.
  /// \tparam L  length of the sequence.
  template <std::size_t S, std::size_t L>
  struct make_index_range_impl
  {
    using type = typename int_range<S, std::make_index_sequence<L>>::type;
  };
  template <std::size_t S>
  struct make_index_range_impl<S, 0>
  {
    using type =std::index_sequence<>;
  };
  template <std::size_t S>
  struct make_index_range_impl<S, -1U>
  {
    using type = std::index_sequence<>;
  };

  /// A static range.
  ///
  /// \tparam S  starting point of the sequence.
  /// \tparam E  end of the sequence, excluded.
  template <std::size_t S, std::size_t E>
  using make_index_range
    = typename make_index_range_impl<S, E - S>::type;


  /*-------------------.
  | concat_sequence.   |
  `-------------------*/

  /// Concatenate two static sequences of size_t.
  template <typename S1, typename S2>
  struct concat_index_sequence;

  template <std::size_t... I1, std::size_t... I2>
  struct concat_index_sequence<std::index_sequence<I1...>,
                               std::index_sequence<I2...>>
  {
    using type = std::index_sequence<I1..., I2...>;
  };

  template <typename S1, typename S2>
  using concat_sequence = typename concat_index_sequence<S1, S2>::type;


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
    = typename index_sequence_difference<S1, S2>::type;

  template <std::size_t I1_1, std::size_t... I1, std::size_t... I2>
  struct index_sequence_difference<std::index_sequence<I1_1, I1...>,
                                   std::index_sequence<I1_1, I2...>>
  {
    using type = sequence_difference<std::index_sequence<I1...>,
                                     std::index_sequence<I2...>>;
  };

  template <std::size_t I1_1, std::size_t I2_1,
            std::size_t... I1, std::size_t... I2>
  struct index_sequence_difference<std::index_sequence<I1_1, I1...>,
                                   std::index_sequence<I2_1, I2...>>
  {
    using type =
      concat_sequence<std::index_sequence<I1_1>,
                      sequence_difference<std::index_sequence<I1...>,
                                          std::index_sequence<I2_1, I2...>>>;
  };

  template <std::size_t I1_1, std::size_t... I1>
  struct index_sequence_difference<std::index_sequence<I1_1, I1...>,
                                   std::index_sequence<>>
  {
    using type =
      concat_sequence<std::index_sequence<I1_1>,
                      sequence_difference<std::index_sequence<I1...>,
                                          std::index_sequence<>>>;
  };

  template <>
  struct index_sequence_difference<std::index_sequence<>, std::index_sequence<>>
  {
    using type = std::index_sequence<>;
  };


  /*--------------------.
  | punched_sequence.   |
  `--------------------*/

  /// An index sequence with a gap.
  template <std::size_t N, std::size_t Gap>
  using punched_sequence
    = concat_sequence<make_index_range<0, Gap>,
                      make_index_range<Gap + 1, N>>;


  /*--------.
  | for_.   |
  `--------*/

  /// Run function \a f on each member of \a ts.
  template <typename Fun, typename... Ts>
  void
  for_(const std::tuple<Ts...>& ts, Fun f)
  {
    for_(f, ts, std::make_index_sequence<sizeof...(Ts)>());
  }

  template <typename Fun, typename... Ts, size_t... I>
  void
  for_(Fun f,
       const std::tuple<Ts...>& ts,
       std::index_sequence<I...>)
  {
    using swallow = int[];
    (void) swallow{ (f(std::get<I>(ts)), 0)... };
  }


  /// Map a function on a tuple, return tuple of the results.
  template <typename Fun, typename... Ts>
  auto
  map(const std::tuple<Ts...>& ts, Fun f)
    -> decltype(map_tuple_(f, ts, std::make_index_sequence<sizeof...(Ts)>()))
  {
    return map_impl_(f, ts, std::make_index_sequence<sizeof...(Ts)>());
  }

  template <typename Fun, typename... Ts, size_t... I>
  auto
  map_impl_(Fun f,
            const std::tuple<Ts...>& ts,
            std::index_sequence<I...>)
  {
    return std::make_tuple(f(std::get<I>(ts))...);
  }


  /// Unpack a tuple, and pass its content as argument to a funtion.
  template <typename Fun, typename... Args>
  auto
  apply(Fun f, const std::tuple<Args...>& args)
    // Return type needed by G++ 5.
    -> decltype(apply_impl_(f, args, std::make_index_sequence<sizeof...(Args)>()))
  {
    return apply_impl_(f, args, std::make_index_sequence<sizeof...(Args)>());
  }

  template <typename Fun, typename... Args, size_t... I>
  auto
  apply_impl_(Fun f,
              const std::tuple<Args...>& args,
              std::index_sequence<I...>)
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
    return apply_impl_(funs, args, std::make_index_sequence<sizeof...(Funs)>());
  }

  /// Apply a tuple of functions on a tuple of arguments, return the
  /// tuple of results.
  template <typename... Funs, typename... Args, size_t... I>
  auto
  apply_impl_(const std::tuple<Funs...>& funs,
              const std::tuple<Args...>& args,
              std::index_sequence<I...>)
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
                       std::make_index_sequence<sizeof...(Objs)>());
  }

  template <typename Fun, typename... Objs, typename... Args, size_t... I>
  auto
  apply_impl_(Fun fun,
              const std::tuple<Objs...>& objs,
              const std::tuple<Args...>& args,
              std::index_sequence<I...>)
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
               std::index_sequence<I...>)
  {
    cross(f, std::get<I>(ts)...);
  }

  template <typename Fun, typename... Ts>
  void
  cross_tuple(Fun f,
              const std::tuple<Ts...>& ts)
  {
    cross_tuple_(f, ts, std::make_index_sequence<sizeof...(Ts)>());
  }



  /*----------------.
  | reverse_tuple.  |
  `----------------*/

  template <typename... Ts>
  auto
  reverse_tuple(const std::tuple<Ts...>& t)
    -> decltype(reverse_tuple(t, std::make_index_sequence<sizeof...(Ts)>()))
  {
    return reverse_tuple(t, std::make_index_sequence<sizeof...(Ts)>());
  }

  template <typename... Ts, std::size_t... I>
  auto
  reverse_tuple(const std::tuple<Ts...>& t, std::index_sequence<I...>)
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
    using indices_t = std::make_index_sequence<sizeof...(Elements)>;

    std::size_t operator()(const value_t& v) const
    {
      return hash_(v, indices_t{});
    }

  private:
    template <std::size_t... I>
    static std::size_t
    hash_(const value_t& v, std::index_sequence<I...>)
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
