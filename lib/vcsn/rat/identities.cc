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
      switch (i)
        {
        case identities::trivial:
          return "trivial";
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
      if (buf == "trivial")
        ids = identities::trivial;
      else if (buf == "series")
        ids = identities::series;
      else
        fail_reading(is, "invalid identities: ", buf);
      return is;
    }

    identities meet(identities i1, identities i2)
    {
      return std::max(i1, i2);
    }

  } // namespace rat

} // namespace vcsn
