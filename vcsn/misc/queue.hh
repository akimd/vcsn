#pragma once

#include <queue>

#include <boost/range/algorithm/find.hpp>

#include <vcsn/misc/attributes.hh>

namespace vcsn
{
  namespace detail
  {
    /// The content of \a cont as a queue.
    template <typename Range>
    std::queue<typename Range::value_type>
    make_queue(const Range& range)
    {
      auto res = std::queue<typename Range::value_type>{};
      for (auto s: range)
        res.emplace(s);
      return res;
     }
  }

  /// Whether \a e is member of \a s.
  template <typename T, typename Container>
  ATTRIBUTE_PURE
  bool
  has(const std::queue<T, Container>& s, const T& e)
  {
    return boost::find(s, e) != std::end(s);
  }
}
