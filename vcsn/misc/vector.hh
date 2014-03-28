#ifndef VCSN_MISC_VECTOR_HH
# define VCSN_MISC_VECTOR_HH

# include <vector>

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
  }
}

#endif // !VCSN_MISC_VECTOR_HH
