#include <lib/vcsn/dot/driver.hh>

#include <cassert>
#include <cstring> // strerror
#include <sstream>

#include <lib/vcsn/dot/parse.hh>
#include <lib/vcsn/dot/scan.hh>
#include <lib/vcsn/rat/caret.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/raise.hh>

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
            require(bool(edit_),
                    "missing vcsn_context definition");
            res = edit_->result();
            edit_->reset();
          }
        scanner_->scan_close_();
        edit_ = nullptr;
        return res;
      }

      void
      driver::setup_(const location_t& l, const std::string& ctx)
      {
        auto c = vcsn::dyn::context{};
        try
          {
            c = vcsn::dyn::make_context(ctx);
          }
        catch (std::runtime_error& e)
          {
            error(l, e.what());
          }
        if (c)
          {
            edit_.reset(vcsn::dyn::make_automaton_editor(c));
            edit_->set_separator(',');
          }
      }

      void
      driver::error(const location_t& l, const std::string& m)
      {
        std::ostringstream er;
        er  << l << ": " << m;
        detail::print_caret(scanner_->yyinput_stream(), er, l);
        if (!!getenv("YYDEBUG"))
          std::cerr << "ERROR: " << er.str() << std::endl;
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
