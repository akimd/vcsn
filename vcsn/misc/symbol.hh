#pragma once

#include <vcsn/config.hh>

#include <boost/flyweight.hpp>
#if VCSN_HAVE_VISIBILITY_HIDDEN
# include <boost/flyweight/intermodule_holder.hpp>
#endif
#include <boost/flyweight/no_tracking.hpp>

namespace vcsn
{
  /// An internalized string.
  using symbol
    = boost::flyweight<std::string
                       , boost::flyweights::no_tracking
#if VCSN_HAVE_VISIBILITY_HIDDEN
                       // See the comments in demo/prod-eval.cc.
                       , boost::flyweights::intermodule_holder
#endif
                       >;

  inline
  std::string operator+(symbol l, const char* r)
  {
    return l.get() + r;
  }

  inline
  std::string operator+(const char* l, symbol r)
  {
    return l + r.get();
  }

  inline
  std::string operator+(symbol l, const std::string& r)
  {
    return l.get() + r;
  }

  inline
  std::string operator+(const std::string& l, symbol r)
  {
    return l + r.get();
  }
}

namespace std
{
  template <>
  struct hash<vcsn::symbol>
  {
    size_t operator()(const vcsn::symbol& ss) const
    {
      // http://lists.boost.org/boost-users/2013/03/78007.php
      hash<const void*> hasher;
      return hasher(&ss.get());
    }
  };
}
