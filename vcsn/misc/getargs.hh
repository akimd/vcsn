#pragma once

#include <sstream>

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include <vcsn/misc/export.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn LIBVCSN_API
{

  /// Find a correspondance in a map.  Meant for string to enum mostly.
  ///
  /// \param kind  the nature of arguments, e.g., "identities"
  /// \param map   a map from keys to values.
  /// \param key   the value to look for.
  template <typename C>
  typename C::mapped_type
  getargs(const std::string& kind, const C& map, const std::string& key)
  {
    auto i = map.find(key);
    if (i == end(map))
      {
        std::ostringstream o;
        // Retrieve all keys
        boost::copy(map | boost::adaptors::map_keys,
                    std::ostream_iterator<std::string>(o, " "));
        raise("invalid ", kind, ": ", key, ", expected: ", o.str());
      }
    else
      return i->second;
  }
}
