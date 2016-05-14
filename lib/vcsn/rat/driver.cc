#include <sstream>

#include <boost/algorithm/string/predicate.hpp> // boost::algorithm::contains

#include <lib/vcsn/rat/driver.hh>
#include <lib/vcsn/rat/parse.hh>
#include <lib/vcsn/rat/scan.hh>
#include <vcsn/dyn/algos.hh> // make_context
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/stream.hh> // fail_reading

namespace vcsn
{
  namespace rat
  {

    driver::driver(const dyn::context& ctx, rat::identities ids)
      : scanner_(new yyFlexLexer)
      , ids_(ids)
    {
      context(ctx);
    }

    driver::~driver()
    {}

    dyn::context driver::context() const
    {
      return tape_ctx_[tapes_.back()];
    }

    void driver::context(const dyn::context& ctx)
    {
      ctx_ = ctx;
      tape_ctx_.clear();
      auto n = dyn::num_tapes(ctx_);
      if (n)
        for (size_t t = 0; t < n; ++t)
          tape_ctx_.emplace_back(dyn::project(ctx_, t));
      else
        tape_ctx_.emplace_back(ctx_);
    }

    void driver::context(const std::string& ctx)
    {
      context(dyn::make_context(ctx));
    }

    rat::identities driver::identities() const
    {
      return ids_;
    }

    void driver::tape_push()
    {
      tapes_.push_back(tapes_.back());
    }

    void driver::tape_pop()
    {
      tapes_.pop_back();
    }

    void driver::tape_inc(const location& l)
    {
      if (tapes_.back() + 1 < tape_ctx_.size())
        ++tapes_.back();
      else
        throw parser::syntax_error(l, "too many tapes");
    }

    void driver::error(const location& l, const std::string& m)
    {
      std::ostringstream er;
      er << l << ": " << m;
      if (!!getenv("YYDEBUG"))
        std::cerr << er.str() << std::endl;
      errors += (errors.empty() ? "" : "\n") + er.str();
    }

    void driver::invalid(const location& l, const std::string& s)
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
      // weights; for instance "<<a>b>c" uses three (nested) parsers.
      static int debug_level = debug_level_();
      static int nesting = 0;
      p.set_debug_level(debug_level && nesting < debug_level);
      ++nesting;
      if (p.parse())
        result_ = nullptr;
      scanner_->scan_close_();
      --nesting;

      return std::move(result_);
    }

    dyn::label
    driver::make_label(const location& loc, const std::string& s,
                       const dyn::context& ctx)
    {
      try
        {
          std::istringstream is{s};
          auto res = dyn::read_label(ctx, is, "text");
          require(is.peek() == -1,
                  "unexpected trailing characters in: ", s, ": ", is);
          return res;
        }
      catch (const std::exception& e)
        {
          throw parser::syntax_error(loc, e.what());
        }
    }

    dyn::expression
    driver::make_atom(const location& loc, const std::string& s)
    {
      // If there are commas in the label, then have it read as a full
      // label, not a one-tape label.
      //
      // FIXME: Remove once the tuple approach works perfectly.  And
      // simplify make_label accordingly (no dyn::context argument
      // needed).
      if (boost::algorithm::contains(s, ","))
        return dyn::to_expression(ctx_, ids_,
                                  make_label(loc, s, ctx_));
      else
        return dyn::to_expression(context(), ids_,
                                  make_label(loc, s, context()));
    }

    dyn::expression
    driver::make_expression(const location& loc,
                            const class_t& c, bool accept = true)
    {
      try
        {
          // If there are commas in the labels, then have it read as a full
          // label, not a one-tape label.
          //
          // FIXME: Remove once the tuple approach works perfectly.
          if (!c.empty()
              && boost::algorithm::contains(detail::front(c).first, ","))
            return dyn::to_expression(ctx_, ids_, c, accept);
          else
            return dyn::to_expression(context(), ids_, c, accept);
        }
      catch (const std::exception& e)
        {
          throw parser::syntax_error(loc, e.what());
        }
    }

    dyn::weight
    driver::make_weight(const location& loc, const std::string& s)
    {
      try
        {
          std::istringstream is{s};
          auto res = dyn::read_weight(ctx_, is);
          require(is.peek() == -1,
                  "unexpected trailing characters in: ", s, ": ", is);
          return res;
        }
      catch (const std::exception& e)
        {
          throw parser::syntax_error(loc, e.what());
        }
    }
  }
}
