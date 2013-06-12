#include <vcsn/misc/escape.hh>

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace vcsn
{
  std::ostream&
  str_escape(std::ostream& os, const int c)
  {
    std::ios_base::fmtflags flags = os.flags(std::ios_base::hex);
    char fill = os.fill('0');
    switch (c)
      {
      case -1:   os << "<end-of-file>"; break;
      case '\\': os << "\\\\";          break;
      case '"':  os << "\\\"";          break;
      case '\n': os << "\\n";           break;
      default:
        if (0 <= c && c <= 0177 && std::isprint(c))
          os << char(c);
        else
          os << "\\0x" << std::setw(3) << c;
        break;
      }
    os.fill(fill);
    os.flags(flags);
    return os;
  }

  std::string
  str_escape(const int c)
  {
    std::ostringstream o;
    str_escape(o, c);
    return o.str();
  }

  std::ostream&
  str_escape(std::ostream& os, const std::string& str)
  {
    for (auto c: str)
      str_escape(os, c);
    return os;
  }

  std::string
  str_escape(const std::string& s)
  {
    std::ostringstream o;
    str_escape(o, s);
    return o.str();
  }
}
