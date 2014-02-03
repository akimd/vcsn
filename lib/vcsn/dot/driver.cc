#include <cassert>
#include <cstring> // strerror
#include <sstream>

#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/dot/parse.hh>
#include <lib/vcsn/dot/scan.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace detail
  {
    namespace dot
    {

      driver::driver()
        : scanner_(new yyFlexLexer)
        , edit_{nullptr}
      {}

      driver::~driver()
      {}

      auto
      driver::parse(std::istream& is, const location_t& l)
        -> dyn::automaton
      {
        scanner_->scan_open_(is);
        scanner_->loc = l;
        location_ = l;
        // Parser.
        parser p(*this);
        p.set_debug_level(!!getenv("YYDEBUG"));
        dyn::automaton res = nullptr;
        // If success.
        if (p.parse() == 0)
          {
            // If the graph is empty, we have not set up the editor,
            // which did not build an automaton.  Do it now.
            if (!edit_)
              setup_();
            res = edit_->result();
            edit_->reset();
          }
        scanner_->scan_close_();
        edit_ = nullptr;
        return res;
      }

      void
      driver::setup_()
      {
        if (!edit_)
          {
            if (context_.empty())
              throw std::domain_error("no vcsn_context defined");
            auto ctx = vcsn::dyn::make_context(context_);
            edit_.reset(vcsn::dyn::make_automaton_editor(ctx));
            edit_->set_separator(',');
          }
      }

      void
      driver::error(const location_t& l, const std::string& m)
      {
        std::ostringstream er;
        er  << l << ": " << m;
        if (!!getenv("YYDEBUG"))
          std::cerr << er.str() << std::endl;
        errors += (errors.empty() ? "" : "\n") + er.str();
      }

      void
      driver::invalid(const location_t& l, const std::string& s)
      {
        error(l, "invalid input: " + s);
      }
    }
  }
}
