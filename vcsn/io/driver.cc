#include <cstring> // strerror

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
      std::cerr << l << ": " << m << std::endl;
    }

    void
    driver::invalid(const location& l, const std::string& s)
    {
      error(l, "invalid character: " + s);
    }

    exp*
    driver::parse()
    {
      // Parser.
      parser p(*this);
      p.set_debug_level(!!getenv("YYDEBUG"));
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
    driver::parse_string(const std::string& e)
    {
      scan_open(e);
      return parse();
    }
  }
}
