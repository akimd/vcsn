#ifndef VCSN_MISC_ESCAPE_HH
# define VCSN_MISC_ESCAPE_HH

namespace vcsn
{
  // Output a string, escaping special characters.
  std::ostream&
  str_escape(std::ostream& os, const std::string& str)
  {
    for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
      switch (*i)
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
	  os << *i;
	  break;
	}
    return os;
  }
}

#endif // !VCSN_MISC_ESCAPE_HH
