#include <cctype>
#include <map>

#include <vcsn/core/rat/identities.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/getargs.hh>
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
        case identities::associative:
          return "associative";
        case identities::linear:
          return "linear";
        case identities::distributive:
          return "distributive";
        case identities::trivial:
          return "trivial";
        case identities::none:
          return "none";
        }
      BUILTIN_UNREACHABLE();
    }

    std::ostream& operator<<(std::ostream& os, identities i)
    {
      return os << to_string(i);
    }

    static std::istream& operator>>(std::istream& is, identities::ids_t& ids)
    {
      static const auto map = std::map<std::string, identities::ids_t>
        {
          {"associative",  identities::associative},
          {"auto",         identities::deflt},
          {"binary",       identities::trivial},
          {"default",      identities::deflt},
          {"distributive", identities::distributive},
          {"linear",       identities::linear},
          {"none",         identities::none},
          {"series",       identities::distributive},
          {"trivial",      identities::trivial},
        };

      std::string buf;
      while (is && isalnum(is.peek()))
        buf += is.get();

      ids = getargs("identities", map, buf);
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
