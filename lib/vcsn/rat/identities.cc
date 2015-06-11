#include <cctype>
#include <stdexcept>

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include <vcsn/core/rat/identities.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/initializer_list.hh>
#include <vcsn/misc/map.hh>
#include <vcsn/misc/stream.hh>

namespace vcsn
{

  namespace rat
  {

    std::string to_string(identities i)
    {
      switch (i.ids())
        {
        case identities::trivial:
          return "trivial";
        case identities::associative:
          return "associative";
        case identities::linear:
          return "linear";
        case identities::distributive:
          return "distributive";
        }
      BUILTIN_UNREACHABLE();
    }

    std::ostream& operator<<(std::ostream& os, identities i)
    {
      return os << to_string(i);
    }

    static std::istream& operator>>(std::istream& is, identities::ids_t& ids)
    {
      std::string buf;
      while (is && isalnum(is.peek()))
        buf += is.get();

      static const auto map = std::map<std::string, identities::ids_t>
        {
          {"associative",  identities::associative},
          {"auto",         identities::deflt},
          {"binary",       identities::trivial},
          {"default",      identities::deflt},
          {"distributive", identities::distributive},
          {"linear",       identities::linear},
          {"series",       identities::distributive},
          {"trivial",      identities::trivial},
        };
      auto i = map.find(buf);
      if (i == end(map))
        {
          std::ostringstream o;
          // Retrieve all keys
          boost::copy(map | boost::adaptors::map_keys,
                      std::ostream_iterator<std::string>(o, " "));
          fail_reading(is, "invalid identities: ", buf,
                       ", expected: ", o.str());
        }
      else
        ids = i->second;
      return is;
    }

    std::istream& operator>>(std::istream& is, identities& ids)
    {
      identities::ids_t id;
      is >> id;
      ids = identities{id};
      return is;
    }

    identities meet(identities i1, identities i2)
    {
      return std::max(i1, i2);
    }

  } // namespace rat

} // namespace vcsn
