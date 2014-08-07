#include <stdexcept>

#include <vcsn/core/rat/identities.hh>
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
        default:
          throw std::runtime_error("unknown identities");
        }
    }

    std::ostream& operator<<(std::ostream& os, identities i)
    {
      return os << to_string(i);
    }

    std::istream& operator>>(std::istream& is, identities& i)
    {
      switch (is.peek())
        {
        case 't':
          eat(is, "trivial");
          i = identities::trivial;
          break;
        case 's':
          eat(is, "series");
          i = identities::series;
          break;
        default:
          fail_reading(is, "ill-formed identities");
        }
      return is;
    }

    identities meet(identities i1, identities i2)
    {
      if (i1 == identities::trivial || i2 == identities::trivial)
        return identities::trivial;
      else
        return identities::series;
    }

  } // namespace rat

} // namespace vcsn
