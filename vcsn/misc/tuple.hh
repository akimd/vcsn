#ifndef VCSN_MISC_TUPLE_HH
# define VCSN_MISC_TUPLE_HH

# include <tuple>
# include <vcsn/misc/hash.hh>

namespace vcsn
{

  /// These definitions come in handy every time we define variadic tuples.
  namespace detail
  {
    // See "Pretty-print std::tuple"
    // <http://stackoverflow.com/questions/6245735>.

    // See O(log N) implementation of integer sequence
    // <http://stackoverflow.com/questions/17424477>

    template<std::size_t...> struct index_sequence
    { using type = index_sequence; };

    template<class S1, class S2> struct concat;

    template<std::size_t... I1, std::size_t... I2>
    struct concat<index_sequence<I1...>, index_sequence<I2...>>
      : index_sequence<I1..., (sizeof...(I1)+I2)...>{};

    template<class S1, class S2>
    using Concat = typename concat<S1, S2>::type;

    template<std::size_t N> struct make_index_sequence;
    template<std::size_t N> using GenSeq =
      typename make_index_sequence<N>::type;

    template<std::size_t N>
    struct make_index_sequence : Concat<GenSeq<N/2>, GenSeq<N - N/2>>{};

    template<> struct make_index_sequence<0> : index_sequence<>{};
    template<> struct make_index_sequence<1> : index_sequence<0>{};

    // There is a bug in clang making this one useless...
    // The index sequence generated is always <0>
    // Bug report:
    // http://llvm.org/bugs/show_bug.cgi?id=14858
    //template<class... T>
    //using index_sequence_for = make_index_sequence<sizeof...(T)>;


    // Compile-time logic
    // See:
    // http://stillmoreperfect.blogspot.fr/2010/03/template-metaprogramming-compile-time.html


    // Test if (c) then T1 else T2
    template<bool c, class T1, class T2>
    struct if_c { typedef T1 type; };

    template<class T1, class T2>
    struct if_c<false, T1, T2> { typedef T2 type; };

    template<class C, class T1, class T2>
    struct if_ : if_c<C::value, T1, T2> {};

    // Test if (c) then F1 else F2 and get the value
    template<bool c, class F1, class F2>
    struct eval_if_c : if_c<c, F1, F2>::type {};

    template<class C, class F1, class F2>
    struct eval_if : if_<C, F1, F2>::type {};

    // And condition on several classes
    template<class... F>
    struct and_;

    template<class F1, class... F>
    struct and_<F1, F...> : eval_if<F1, and_<F...>,std::false_type>::type {};

    template<class F1>
    struct and_<F1> : eval_if<F1, std::true_type, std::false_type>::type {};

    template<>
    struct and_<> : std::true_type::type {};

    // Or condition on several classes
    template<class... F>
    struct or_;

    template<class F1, class... F>
    struct or_<F1, F...> : eval_if<F1 ,std::true_type, or_<F...>>::type { };

    template<class F1>
    struct or_<F1> : eval_if<F1, std::true_type, std::false_type>::type { };

    template<>
    struct or_<> : std::true_type::type {};

  }

  // Static evaluation of the 'or' of the template parameters
  template<bool... B>
  constexpr bool any_ () { return detail::or_<std::integral_constant<bool, B>...>::value;}

  // Static evaluation of the 'and' of the template parameters
  template<bool... B>
  constexpr bool all_ () { return detail::and_<std::integral_constant<bool, B>...>::value;}


}

namespace std
{

  /*--------------------.
  | hash(tuple<T...>).  |
  `--------------------*/

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
      for (const auto& e: {std::get<I>(v)...})
        std::hash_combine(res, e);
      return res;
    }
  }; // class
}

#endif // !VCSN_MISC_TUPLE_HH
