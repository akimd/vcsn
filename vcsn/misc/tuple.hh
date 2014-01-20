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

    template<std::size_t...> struct seq
    { using type = seq; };

    template<class S1, class S2> struct concat;

    template<std::size_t... I1, std::size_t... I2>
    struct concat<seq<I1...>, seq<I2...>>
      : seq<I1..., (sizeof...(I1)+I2)...>{};

    template<class S1, class S2>
    using Concat = typename concat<S1, S2>::type;

    template<std::size_t N> struct gen_seq;
    template<std::size_t N> using GenSeq = typename gen_seq<N>::type;

    template<std::size_t N>
    struct gen_seq : Concat<GenSeq<N/2>, GenSeq<N - N/2>>{};

    template<> struct gen_seq<0> : seq<>{};
    template<> struct gen_seq<1> : seq<0>{};
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
    using indices_t = vcsn::detail::gen_seq<sizeof...(Elements)>;

    std::size_t operator()(const value_t& v) const
    {
      return hash_(v, indices_t{});
    }

  private:
    template <std::size_t... I>
    static std::size_t
    hash_(const value_t& v, vcsn::detail::seq<I...>)
    {
      std::size_t res = 0;
      for (const auto& e: {std::get<I>(v)...})
        std::hash_combine(res, e);
      return res;
    }
  }; // class
}

#endif // !VCSN_MISC_TUPLE_HH
