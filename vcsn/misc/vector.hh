#ifndef VCSN_MISC_VECTOR_HH
# define VCSN_MISC_VECTOR_HH

# include <vector>

# include <vcsn/misc/tuple.hh> // make_index_sequence.

namespace vcsn
{

  namespace detail
  {

    /// Variadic Cartesian product of vectors.
    ///
    /// http://stackoverflow.com/questions/13813007/
    template <typename Fun>
    inline void
    cross(Fun f)
    {
      f();
    }

    template<typename Fun, typename H, typename... Ts>
    inline void
    cross(Fun f,
          std::vector<H> const& h,
          std::vector<Ts> const&... ts)
    {
      for (H const& he: h)
        cross([&](Ts const&... ts) { f(he, ts...); }, ts...);
    }

    template<typename Fun, typename... Ts>
    inline void
    cross_tuple(Fun f,
                const std::tuple<Ts...>& ts)
    {
      cross_tuple_(f, ts, make_index_sequence<sizeof...(Ts)>());
    }

    template<typename Fun, typename... Ts, size_t... I>
    inline void
    cross_tuple_(Fun f,
                 const std::tuple<Ts...>& ts,
                 index_sequence<I...>)
    {
      cross(f, std::get<I>(ts)...);
    }
  }
}

#endif // !VCSN_MISC_VECTOR_HH
