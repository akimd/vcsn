#include <cstring> // strerror
#include <sstream>

#include <vcsn/io/driver.hh>
#include <vcsn/io/parse-rat-exp.hh>

namespace vcsn
{
  namespace rat
  {

    driver::driver(const factory& f)
    {
      factory_ = &f;
    }

    void
    driver::error(const location& l, const std::string& m)
    {
      std::stringstream er;
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

    exp*
    driver::parse(const location& l)
    {
      // Parser.
      parser p(*this);
      p.set_debug_level(!!getenv("YYDEBUG"));
      location_ = l;
      if (p.parse())
        result_ = 0;
      scan_close();
      exp* res = 0;
      std::swap(result_, res);
      return res;
    }

    exp*
    driver::parse_file(const std::string& f)
    {
      FILE *yyin = f == "-" ? stdin : fopen(f.c_str(), "r");
      if (!yyin)
        {
          std::cerr << f << ": cannot open: " << strerror(errno) << std::endl;
          exit(1);
        }
      scan_open(yyin);
      auto res = parse();
      if (f != "-")
        fclose(yyin);
      return res;
    }

    exp*
    driver::parse_string(const std::string& e, const location& l)
    {
      scan_open(e);
      return parse(l);
    }
  }
}
