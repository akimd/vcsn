#pragma once

#include <boost/flyweight.hpp>
#include <boost/flyweight/no_tracking.hpp>

namespace std
{
  template <typename T>
  struct hash<boost::flyweight<T, boost::flyweights::no_tracking>>
  {
    using value_type = boost::flyweight<T, boost::flyweights::no_tracking>;
    size_t operator()(const value_type& ss) const
    {
      // http://lists.boost.org/boost-users/2013/03/78007.php
      hash<const void*> hasher;
      return hasher(&ss.get());
    }
  };
}

namespace vcsn
{
  /// An internalized string.
  using symbol = boost::flyweight<std::string, boost::flyweights::no_tracking>;
}
