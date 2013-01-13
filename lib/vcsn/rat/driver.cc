#include <cstring> // strerror
#include <sstream>

#include <vcsn/algos/dyn.hh> // make_ratexpset.
#include <lib/vcsn/rat/driver.hh>
#include <lib/vcsn/rat/parse.hh>

namespace vcsn
{
  namespace rat
  {

    driver::driver(const dyn::context& ctx)
      : context_{ctx}
      , ratexpset_{dyn::make_ratexpset(ctx)}
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
      -> dyn::ratexp
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
        result_ = 0; // FIXME: no warning from GCC?  Should be nullptr.
      scan_close_();
      --nesting;
      dyn::ratexp res = make_ratexp(context_, result_);
      result_ = nullptr;
      return res;
    }

    auto
    driver::parse_file(const std::string& f)
      -> dyn::ratexp
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
      -> dyn::ratexp
    {
      scan_open_(e);
      return parse_(l);
    }
  }
}
