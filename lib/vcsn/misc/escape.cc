#include <vcsn/misc/escape.hh>

#include <cctype>
#include <cstring> // strchr
#include <iomanip>
#include <iostream>
#include <sstream>

namespace vcsn
{
  // Accept int to catch EOF too.
  std::ostream&
  str_escape(std::ostream& os, const int c, const char* special)
  {
    std::ios_base::fmtflags flags = os.flags(std::ios_base::hex);
    char fill = os.fill('0');
    switch (c)
      {
      case -1:   os << "<end-of-file>"; break;
      case '\\': os << "\\\\";          break;
      case '\n': os << "\\n";           break;
      default:
        if (0 <= c && c <= 0177 && std::isprint(c))
          {
            if (special && strchr(special, c))
              os << '\\';
            os << uint8_t(c);
          }
        else
          os << "\\x" << std::setw(2) << c;
        break;
      }
    os.fill(fill);
    os.flags(flags);
    return os;
  }

  std::ostream&
  str_escape(std::ostream& os, char c, const char* special)
  {
    // Turn into an unsigned, otherwise if c has its highest bit set,
    // it will be interpreted as a negative char, which will be mapped
    // to a negative int.  So for instance c = 255 is mapped to
    // \xFFFFFFF instead of \xFF.
    //
    // This happens only when char is "signed char".  Which is common.
    return str_escape(os, int(uint8_t(c)), special);
  }

  std::ostream&
  str_escape(std::ostream& o, const std::string& str, const char* special)
  {
    for (auto c: str)
      str_escape(o, c, special);
    return o;
  }
}
