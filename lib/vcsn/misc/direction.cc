#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/stream.hh>

namespace vcsn
{
  std::string to_string(direction d)
  {
    switch (d)
      {
      case direction::forward:
        return "forward";
      case direction::backward:
        return "backward";
      }
    BUILTIN_UNREACHABLE();
  }

  std::ostream& operator<<(std::ostream& os, direction d)
  {
    return os << to_string(d);
  }

  std::istream& operator>>(std::istream& is, direction& d)
  {
    switch (is.peek())
      {
      case 'b':
        eat(is, "backward");
        d = direction::backward;
        break;
      case 'f':
        eat(is, "forward");
        d = direction::forward;
        break;
      default:
        fail_reading(is, "invalid direction");
      }
    return is;
  }
} // namespace vcsn
