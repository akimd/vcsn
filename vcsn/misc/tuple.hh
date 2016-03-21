#pragma once

#include <iostream>
#include <tuple>

#include <vcsn/misc/functional.hh>
#include <vcsn/misc/type_traits.hh> // bool_constant

namespace vcsn
{

  /// C++14.
  template <size_t I, typename T>
  using tuple_element_t = typename std::tuple_element<I, T>::type;

  // These definitions come in handy every time we define variadic tuples.
  namespace detail
  {

    /*-----------------.
    | index_sequence.  |
    `-----------------*/

    // See "Pretty-print std::tuple"
    // <http://stackoverflow.com/questions/6245735>.

    // See O(log N) implementation of integer sequence
    // <http://stackoverflow.com/questions/17424477>

    template <std::size_t...> struct index_sequence
    { using type = index_sequence; };

    template <typename S1, typename S2> struct concat;

    template <std::size_t... I1, std::size_t... I2>
    struct concat<index_sequence<I1...>, index_sequence<I2...>>
      : index_sequence<I1..., (sizeof...(I1)+I2)...>{};

    template <typename S1, typename S2>
    using Concat = typename concat<S1, S2>::type;

    template <std::size_t N> struct make_index_sequence;
    template <std::size_t N> using GenSeq =
      typename make_index_sequence<N>::type;

    template <std::size_t N>
    struct make_index_sequence : Concat<GenSeq<N/2>, GenSeq<N - N/2>>{};

    template <> struct make_index_sequence<0> : index_sequence<>{};
    template <> struct make_index_sequence<1> : index_sequence<0>{};

    template <std::size_t off, typename S2> struct int_range;

    template <std::size_t off, std::size_t... I>
    struct int_range<off, index_sequence<I...>>
      : index_sequence<I + off...>{};


    /*--------------------.
    | make_index_range.   |
    `--------------------*/

    template <std::size_t S, std::size_t L>
    struct make_index_range
      : int_range<S, typename make_index_sequence<L>::type>
    {};

    template <std::size_t S>
    struct make_index_range<S, 0> : index_sequence<>{};
    template <std::size_t S>
    struct make_index_range<S, -1U> : index_sequence<>{};

    template <std::size_t S, std::size_t L>
    using make_index_range_t = typename make_index_range<S, L>::type;

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

    /**
     * Get the list containing all the elements of I1 (contiguous sequence from
     * 0 to N) not present in I2 (arbitrary sequence, sorted).
     */
    template <typename S1, typename S2>
    struct index_sequence_difference;

    template <std::size_t I1_1, std::size_t... I1, std::size_t... I2>
    struct index_sequence_difference<index_sequence<I1_1, I1...>,
                                     index_sequence<I1_1, I2...>>
    {
      using type =
       typename index_sequence_difference<index_sequence<I1...>,
                                          index_sequence<I2...>>::type;
    };

    template <std::size_t I1_1, std::size_t I2_1,
              std::size_t... I1, std::size_t... I2>
    struct index_sequence_difference<index_sequence<I1_1, I1...>,
                                     index_sequence<I2_1, I2...>>
    {
      using type =
        typename concat_index_sequence<index_sequence<I1_1>,
                typename index_sequence_difference<index_sequence<I1...>,
                                          index_sequence<I2_1, I2...>>::type>::type;
    };

    template <std::size_t I1_1, std::size_t... I1>
    struct index_sequence_difference<index_sequence<I1_1, I1...>, index_sequence<>>
    {
      using type =
        typename concat_index_sequence<index_sequence<I1_1>,
                typename index_sequence_difference<index_sequence<I1...>,
                                          index_sequence<>>::type>::type;
    };

    template <>
    struct index_sequence_difference<index_sequence<>, index_sequence<>>
    {
      using type = typename index_sequence<>::type;
    };

    template <typename S1, typename S2>
    using sequence_difference
      = typename index_sequence_difference<typename S1::type,
                                           typename S2::type>::type;

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
      return map_tuple_(f, ts, make_index_sequence<sizeof...(Ts)>());
    }

    template <typename Fun, typename... Ts, size_t... I>
    auto
    map_tuple_(Fun f,
               const std::tuple<Ts...>& ts,
               index_sequence<I...>)
      -> decltype(map_variadic_(f, std::get<I>(ts)...))
    {
      return map_variadic_(f, std::get<I>(ts)...);
    }

    template <typename Fun>
    auto
    map_variadic_(Fun)
      -> decltype(std::make_tuple())
    {
      return std::make_tuple();
    }

    template <typename Fun, typename T, typename... Ts>
    auto
    map_variadic_(Fun f, T t, Ts&&... ts)
      -> decltype(std::tuple_cat(std::make_tuple(f(t)),
                                 map_variadic_(f, ts...)))
    {
      // Enforce evaluation order from left to right.
      auto r = f(t);
      return std::tuple_cat(std::make_tuple(r), map_variadic_(f, ts...));
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


    // Compile-time logic
    // See:
    // http://stillmoreperfect.blogspot.fr/2010/03/template-metaprogramming-compile-time.html

    // Test if (c) then T1 else T2
    template <bool c, typename T1, typename T2>
    struct if_c { typedef T1 type; };

    template <typename T1, typename T2>
    struct if_c<false, T1, T2> { typedef T2 type; };

    template <typename C, typename T1, typename T2>
    struct if_ : if_c<C::value, T1, T2> {};

    /// Test if (c) then F1 else F2 and get the value.
    template <bool c, typename F1, typename F2>
    struct eval_if_c : if_c<c, F1, F2>::type {};

    template <typename C, typename F1, typename F2>
    struct eval_if : if_<C, F1, F2>::type {};

    /// And condition on several typenames.
    template <typename... F>
    struct and_;

    template <typename F1, typename... F>
    struct and_<F1, F...> : eval_if<F1, and_<F...>, std::false_type>::type {};

    template <typename F1>
    struct and_<F1> : eval_if<F1, std::true_type, std::false_type>::type {};

    template <>
    struct and_<> : std::true_type::type {};

    /// Or condition on several typenames.
    template <typename... F>
    struct or_;

    template <typename F1, typename... F>
    struct or_<F1, F...> : eval_if<F1, std::true_type, or_<F...>>::type { };

    template <typename F1>
    struct or_<F1> : eval_if<F1, std::true_type, std::false_type>::type { };

    template <>
    struct or_<> : std::true_type::type {};
  }

  /// Static evaluation of the 'or' of the template parameters
  template <bool... B>
  constexpr bool any_()
  {
    return detail::or_<bool_constant<B>...>::value;
  }

  // Static evaluation of the 'and' of the template parameters
  template <bool... B>
  constexpr bool all_()
  {
    return detail::and_<bool_constant<B>...>::value;
  }

  /// Whether all the `values` evaluate as true.
  template <typename... Bool>
  bool all(Bool&&... values)
  {
    bool res = true;
    using swallow = int[];
    (void) swallow
    {
      (res = res && values, 0)...
    };
    return res;
  }
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
