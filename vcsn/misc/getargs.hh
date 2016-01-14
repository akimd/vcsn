#pragma once

#include <sstream>

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/variant.hpp>

#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/map.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn LIBVCSN_API
{
  namespace detail
  {
    template <typename C>
    ATTRIBUTE_NORETURN
    void invalid_argument(const std::string& kind, const std::string& key,
                          const C& map)
    {
      std::ostringstream o;
      // Retrieve all keys
      boost::copy(map | boost::adaptors::map_keys,
                  std::ostream_iterator<std::string>(o, " "));
      raise("invalid ", kind, ": ", key, ", expected: ", o.str());
    }
  }

  /// A mapping from strings to Values.
  template <typename Value>
  struct getarg
  {
    // FIXME: Boost::ProgramOptions might be a better bet.
    using value_t = Value;
    using data_t = boost::variant<std::string, Value>;
    using map_t = std::map<std::string, data_t>;

    getarg(const std::string& kind,
           std::initializer_list<typename map_t::value_type> args)
      : kind_{kind}
      , map_{args}
    {}

    /// The value associated to \a s.
    const value_t& operator[](std::string s) const
    {
      while (true)
        {
          auto i = map_.find(s);
          if (i == end(map_))
            detail::invalid_argument(kind_, s, map_);
          else if (auto* res = boost::get<value_t>(&i->second))
            return *res;
          else
            s = boost::get<std::string>(i->second);
        }
    }

    /// The nature of the selection.  Used in error messages.
    std::string kind_;
    /// string -> (string | value).
    map_t map_;
  };
}
