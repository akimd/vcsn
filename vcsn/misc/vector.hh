#pragma once

#include <algorithm>
#include <vector>

#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/tuple.hh> // make_index_sequence.

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

    /// Return the content of \a cont as a vector.
    template <typename Cont>
    std::vector<typename Cont::value_type>
    to_vector(const Cont& cont)
    {
      return {std::begin(cont), std::end(cont)};
    }
  }

  /// Convenience wrapper around std::find.
  template <typename T, typename Alloc>
  ATTRIBUTE_PURE
  auto
  find(const std::vector<T, Alloc>& s, const T& e)
    -> typename std::vector<T, Alloc>::const_iterator
  {
    return std::find(std::begin(s), std::end(s), e);
  }

  /// Whether \a e is member of \a s.
  template <typename T, typename Alloc>
  ATTRIBUTE_PURE
  bool
  has(const std::vector<T, Alloc>& s, const T& e)
  {
    return find(s, e) != std::end(s);
  }
}

