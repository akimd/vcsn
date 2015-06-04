#include <cctype>
#include <stdexcept>

#include <vcsn/core/rat/identities.hh>
#include <vcsn/misc/builtins.hh>
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
        case identities::traditional:
          return "traditional";
        case identities::series:
          return "series";
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

      if (buf == "trivial" || buf == "binary")
        ids = identities::trivial;
      else if (buf == "associative")
        ids = identities::associative;
      else if (buf == "traditional")
        ids = identities::traditional;
      else if (buf == "series")
        ids = identities::series;
      else
        fail_reading(is, "invalid identities: ", buf);

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
