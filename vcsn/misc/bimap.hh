#pragma once

#include <boost/bimap.hpp>

#include <vcsn/misc/functional.hh>

namespace vcsn
{

  /// Whether \a e is in \a s.
  template <typename Map, typename Key>
  auto
  has(const Map& s, const Key& e)
    -> decltype(s.find(e) != std::end(s))
  {
    return s.find(e) != std::end(s);
  }

}
