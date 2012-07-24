#include <iostream>
#include <vcsn/misc/escape.hh>

namespace vcsn
{
  std::ostream&
  str_escape(std::ostream& os, const std::string& str)
  {
    for (auto c: str)
      switch (c)
	{
	case '\\':
	  os << "\\\\";
	  break;
	case '"':
	  os << "\\\"";
	  break;
	case '\n':
	  os << "\\n";
	  break;
	default:
	  os << c;
	  break;
	}
    return os;
  }
}
