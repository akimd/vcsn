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

    exp*
    driver::parse()
    {
      // Parser.
      parser p(*this);
      p.set_debug_level(!!getenv("YYDEBUG"));
      extern exp* result;
      exp* res = 0;
      std::swap(result, res);
      if (p.parse())
        result = 0;
      scan_close();
      std::swap(result, res);
      return res;
    }

    exp*
    driver::parse_file(const std::string& f)
    {
      scan_file(f);
      return parse();
    }

    exp*
    driver::parse_string(const std::string& e)
    {
      scan_string(e);
      return parse();
    }
  }
}
