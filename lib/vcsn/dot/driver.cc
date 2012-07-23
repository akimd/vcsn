#include <cstring> // strerror
#include <sstream>

#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/dot/parse.hh>

namespace vcsn
{
  namespace dot
  {

    driver::driver()
    {
    }

    void
    driver::error(const location& l, const std::string& m)
    {
      std::ostringstream er;
      er  << l << ": " << m;
      if (errors.empty())
        errors += (errors.empty() ? "" : "\n") + er.str();
      // std::cerr << l << ": " << m << std::endl;
    }

    void
    driver::invalid(const location& l, const std::string& s)
    {
      error(l, "invalid character: " + s);
    }

    auto
    driver::parse(const location& l)
      -> exp_t
    {
      location_ = l;
      // Parser.
      parser p(*this);
      p.set_debug_level(!!getenv("YYDEBUG"));
      auto res = p.parse();
      scan_close();
    }

    auto
    driver::parse_file(const std::string& f)
      -> exp_t
    {
      FILE *yyin = f == "-" ? stdin : fopen(f.c_str(), "r");
      if (!yyin)
        {
          std::cerr << f << ": cannot open: " << strerror(errno) << std::endl;
          exit(1);
        }
      scan_open(yyin);
      //auto res = 
      parse();
      if (f != "-")
        fclose(yyin);
      //      return res;
    }

    auto
    driver::parse_string(const std::string& e, const location& l)
      -> exp_t
    {
      scan_open(e);
      return parse(l);
    }
  }
}
