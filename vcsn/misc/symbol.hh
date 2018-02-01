#pragma once

#include <boost/flyweight.hpp>
#include <boost/flyweight/intermodule_holder.hpp>
#include <boost/flyweight/no_tracking.hpp>

namespace vcsn
{
  /// An internalized string.
  ///
  /// We do use different modules, since we dlopen plugins.  For a
  /// while, we had the illusion that it worked properly, but that's
  /// only because we were using the default operator< which actually
  /// compares the strings, not the address of the symbols.  So we did
  /// have synonymous symbols, which the code gracefully accepted.
  ///
  /// Play it safe and make our symbols unique.
  using symbol
    = boost::flyweight<std::string,
                       boost::flyweights::no_tracking,
                       boost::flyweights::intermodule_holder>;
}

namespace boost
{
  namespace flyweights
  {
    inline
    std::string operator+(vcsn::symbol l, const char* r)
    {
      return l.get() + r;
    }

    inline
    std::string operator+(const char* l, vcsn::symbol r)
    {
      return l + r.get();
    }

    inline
    std::string operator+(vcsn::symbol l, const std::string& r)
    {
      return l.get() + r;
    }

    inline
    std::string operator+(const std::string& l, vcsn::symbol r)
    {
      return l + r.get();
    }
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
      auto hasher = hash<const void*>{};
      return hasher(&ss.get());
    }
  };
}
