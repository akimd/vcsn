#include <vcsn/misc/escape.hh>

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace vcsn
{
  // Accept int to catch EOF too.
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
          os << "\\x" << std::setw(2) << c;
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
      // Turn into an unsigned, otherwise if c has its highest but
      // set, will be interpreted as a negative char, which will be
      // mapped to a negtive int.  So for instance c = 255 is mapped
      // to \xFFFFFFF instead of 255.
      //
      // This happens only when char is "signed char".  Which is
      // common.
      str_escape(os, uint8_t(c));
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
