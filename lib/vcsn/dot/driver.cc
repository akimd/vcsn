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

namespace vcsn::detail::dot
{
  driver::driver()
    : scanner_{std::make_unique<yyFlexLexer>()}
    , edit_{nullptr}
  {}

  driver::~driver() = default;

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
    auto status = p.parse();
    if (!errors.empty())
      raise(errors);
    if (status == 0)
      {
        require(bool(edit_),
                "no vcsn_context defined");
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
    try
      {
        auto c = vcsn::dyn::make_context(ctx);
        edit_.reset(vcsn::dyn::make_automaton_editor(c));
        edit_->set_separator(',');
      }
    catch (std::runtime_error& e)
      {
        throw parser::syntax_error(l, e.what());
      }
  }

  bool
  driver::has_edit_(const location_t& l)
  {
    if (!require_context_done_)
      {
        require_context_done_ = true;
        if (!edit_)
          throw parser::syntax_error(l,
                                     "no vcsn_context defined");
      }
    return bool(edit_);
  }

  void
  driver::error(const location_t& l, const std::string& m)
  {
    auto er = std::ostringstream{};
    er  << l << ": " << m;
    detail::print_caret(scanner_->yyinput_stream(), er, l);
    if (!!getenv("YYDEBUG"))
      std::cerr << "ERROR: " << er.str() << std::endl;
    errors += (errors.empty() ? "" : "\n") + er.str();
  }

  void
  driver::invalid(const location_t& l, const std::string& s)
  {
    throw parser::syntax_error(l, "invalid input: " + s);
  }
}
