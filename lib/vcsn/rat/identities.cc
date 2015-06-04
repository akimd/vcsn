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

    std::istream& operator>>(std::istream& is, identities& ids)
    {
      std::string buf;
      while (is && isalnum(is.peek()))
        buf += is.get();

      identities::ids_t id;
      if (buf == "trivial")
        id = identities::trivial;
      else if (buf == "traditional")
        id = identities::traditional;
      else if (buf == "series")
        id = identities::series;
      else
        fail_reading(is, "invalid identities: ", buf);

      ids = identities{id};
      return is;
    }

    identities meet(identities i1, identities i2)
    {
      return std::max(i1, i2);
    }

  } // namespace rat

} // namespace vcsn
