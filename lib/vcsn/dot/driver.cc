#include <cassert>
#include <cstring> // strerror
#include <sstream>
#include <stdexcept>

#include <vcsn/dyn/automaton.hh>
#include <lib/vcsn/dot/driver.hh>
#include <lib/vcsn/dot/parse.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/algos/edit-automaton.hh>

namespace vcsn
{
  namespace detail
  {
    namespace dot
    {

      driver::driver()
        : edit_{nullptr}
      {}

      auto
      driver::parse_(const location_t& l)
        -> automaton_t
      {
        location_ = l;
        // Parser.
        parser p(*this);
        p.set_debug_level(!!getenv("YYDEBUG"));
        automaton_t res = nullptr;
        // If success.
        if (p.parse() == 0)
          {
            // If the graph is empty, we have not set up the editor,
            // which did not build an automaton.  Do it now.
            if (!edit_)
              setup_();
            res.reset(edit_->result());
            edit_->reset();
          }
        delete edit_;
        edit_ = nullptr;
        scan_close_();
        return res;
      }

      auto
      driver::parse_file(const std::string& f)
        -> automaton_t
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
      driver::parse_string(const std::string& e, const location_t& l)
        -> automaton_t
      {
        scan_open_(e);
        return parse_(l);
      }

      void
      driver::setup_()
      {
        if (!edit_)
          {
            if (context_.empty())
              throw std::domain_error("no vcsn_context defined");
            auto ctx = vcsn::dyn::make_context(context_);
            edit_ = make_automaton_editor(ctx);
            assert(edit_);
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
