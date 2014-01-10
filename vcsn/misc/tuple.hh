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
    template<std::size_t...> struct seq{};

    template<std::size_t N, std::size_t... Is>
    struct gen_seq : gen_seq<N-1, N-1, Is...>{};

    template<std::size_t... Is>
    struct gen_seq<0, Is...> : seq<Is...>{};
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
