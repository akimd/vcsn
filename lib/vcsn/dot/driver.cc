#include <cstring> // strerror
#include <sstream>

#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/dot/parse.hh>

namespace vcsn
{
  namespace dot
  {

    driver::driver()
      : kratexpset_{nullptr}
    {}

    void
    driver::error(const location& l, const std::string& m)
    {
      std::ostringstream er;
      er  << l << ": " << m;
      if (!!getenv("YYDEBUG"))
        std::cerr << er.str() << std::endl;
      errors += (errors.empty() ? "" : "\n") + er.str();
    }

    void
    driver::invalid(const location& l, const std::string& s)
    {
      error(l, "invalid character: " + s);
    }

    auto
    driver::parse(const location& l)
      -> automaton_t
    {
      location_ = l;
      // Parser.
      parser p(*this);
      p.set_debug_level(!!getenv("YYDEBUG"));
      p.parse();
      scan_close();
      return {ctx::char_b_lal{}};
    }

    auto
    driver::parse_file(const std::string& f)
      -> automaton_t
    {
      FILE *yyin = f == "-" ? stdin : fopen(f.c_str(), "r");
      if (!yyin)
        {
          std::cerr << f << ": cannot open: " << strerror(errno) << std::endl;
          exit(1);
        }
      scan_open(yyin);
      parse();
      if (f != "-")
        fclose(yyin);
      return {ctx::char_b_lal{}};
    }

    auto
    driver::parse_string(const std::string& e, const location& l)
      -> automaton_t
    {
      scan_open(e);
      return parse(l);
    }
  }
}
