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
  }

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
