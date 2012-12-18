#include <cstring> // strerror
#include <sstream>

#include <vcsn/algos/dyn.hh> // make_ratexpset.
#include <lib/vcsn/rat/driver.hh>
#include <lib/vcsn/rat/parse.hh>

namespace vcsn
{
  namespace rat
  {

    driver::driver(const abstract_ratexpset& f)
      : ratexpset_{&f}
    {}

    driver::driver(const dyn::context& ctx)
      : ratexpset_{make_ratexpset(ctx)}
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
      error(l, "invalid input: " + s);
    }

    /// The nesting limit for parser traces, as specified per
    /// $YYDEBUG.
    static int
    debug_level_()
    {
      int res = 0;
      if (const char* cp = getenv("YYDEBUG"))
        {
          res = 1;
          std::istringstream s{cp};
          s >> res;
        }
      return res;
    }

    auto
    driver::parse_(const location& l)
      -> exp_t
    {
      location_ = l;
      // Parser.
      parser p(*this);
      // If the number of nested parser invocations is less than
      // $YYDEBUG, be verbose.  Recursive parsings are used for
      // weights; for instance "{{a}b}c" uses three (nested) parsers.
      static int debug_level = debug_level_();
      static int nesting = 0;
      p.set_debug_level(debug_level && nesting < debug_level);
      ++nesting;
      if (p.parse())
        result_ = 0;
      scan_close_();
      --nesting;
      exp_t res = 0;
      std::swap(result_, res);
      return res;
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
      scan_open_(yyin);
      auto res = parse_();
      if (f != "-")
        fclose(yyin);
      return res;
    }

    auto
    driver::parse_string(const std::string& e, const location& l)
      -> exp_t
    {
      scan_open_(e);
      return parse_(l);
    }
  }
}
