#include <cctype>
#include <map>

#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/map.hh>
#include <vcsn/misc/stream.hh>

namespace vcsn
{

  format::format(const std::string& f)
  {
    static const auto map = std::map<std::string, format_t>
      {
        {"auto",       deflt},
        {"default",    deflt},
        {"latex",      latex},
        {"raw",        raw},
        {"sname",      sname},
        {"text",       text},
        {"utf8",       utf8},
      };
    format_ = getargs("format", map, f);
  }

  std::string to_string(format f)
  {
    switch (f.kind())
      {
      case format::latex:
        return "latex";
      case format::raw:
        return "raw";
      case format::sname:
        return "sname";
      case format::text:
        return "text";
      case format::utf8:
        return "utf8";
      }
    BUILTIN_UNREACHABLE();
  }

  std::ostream& operator<<(std::ostream& os, format i)
  {
    return os << to_string(i);
  }

  std::istream& operator>>(std::istream& is, format& fmt)
  {
    std::string buf;
    while (is && isalnum(is.peek()))
      buf += is.get();
    fmt = format(buf);
    return is;
  }

} // namespace vcsn
