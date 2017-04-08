#pragma once

#include <algorithm>
#include <vector>

#include <boost/range/algorithm/find.hpp>

#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/functional.hh> // hash_combine

namespace vcsn
{
  namespace detail
  {
    /// The content of \a cont as a vector.
    template <typename Cont>
    std::vector<typename Cont::value_type>
    make_vector(const Cont& cont)
    {
      using std::begin;
      using std::end;
      return {begin(cont), end(cont)};
    }
  }

  /// Whether \a e is member of \a s.
  template <typename T, typename Alloc>
  ATTRIBUTE_PURE
  bool
  has(const std::vector<T, Alloc>& s, const T& e)
  {
    return boost::find(s, e) != std::end(s);
  }
}

namespace std
{
  template <typename T, typename Alloc>
  struct hash<vector<T, Alloc>>
  {
    size_t operator()(const std::vector<T, Alloc>& vs) const
    {
      auto res = size_t{0};
      for (const auto& v: vs)
        vcsn::hash_combine(res, v);
      return res;
    }
  };
}
