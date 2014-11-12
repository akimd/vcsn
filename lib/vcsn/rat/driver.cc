#include <cstring> // strerror
#include <sstream>

#include <vcsn/dyn/algos.hh> // make_expressionset.
#include <lib/vcsn/rat/driver.hh>
#include <lib/vcsn/rat/parse.hh>

#include <lib/vcsn/rat/scan.hh>

namespace vcsn
{
  namespace rat
  {

    driver::driver(const dyn::expressionset& rs)
      : scanner_(new yyFlexLexer)
    {
      expressionset(rs);
    }

    driver::~driver()
    {}

    void driver::expressionset(const dyn::expressionset& rs)
    {
      expressionset_ = rs;
    }

    void driver::context(const std::string& ctx)
    {
      rat::identities ids = rat::identities::trivial;
      expressionset(dyn::make_expressionset(dyn::make_context(ctx), ids));
    }

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
    driver::parse(std::istream& is, const location& l)
      -> dyn::expression
    {
      scanner_->scan_open_(is);
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
        result_ = nullptr;
      scanner_->scan_close_();
      --nesting;

      dyn::expression res = nullptr;
      if (result_)
        {
          res = expressionset_->make_expression(result_);
          result_ = nullptr;
        }
      return res;
    }
  }
}
