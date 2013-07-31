#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  /*------------------------.
  | lazy_automaton_editor.  |
  `------------------------*/

  /// Add transitions from \a src to \a dst, labeled by \a entry.
  void
  lazy_automaton_editor::add_entry(const string_t& src,
                                   const string_t& dst,
                                   const string_t& entry)
  {
    transitions_.emplace_back(src, entry, dst);
    if (entry == "\\e")
      {
        is_lan_ = true;
        return;
      }
    // We can't iterate on string_t
    std::string s = entry;
    for (auto c: s)
      letters_.emplace(c);
    if (1 < entry.get().size())
      is_law_ = true;
  }

  /// Create ctx and return the built automaton.
  dyn::detail::abstract_automaton*
  lazy_automaton_editor::result()
  {
    std::string ctx = is_law_ ? "law" : (is_lan_ ? "lan" : "lal");
    ctx += "_char(";
    for (auto l: letters_)
      ctx += l;
    ctx += ")_b";
    auto c = vcsn::dyn::make_context(ctx);
    auto edit = vcsn::dyn::make_automaton_editor(c);

    for (auto s: states_)
      edit->add_state(s);

    for (auto t: transitions_)
      edit->add_entry(std::get<0>(t), std::get<2>(t), std::get<1>(t));

    for (auto p: initial_states_)
      edit->add_initial(p.first, p.second);

    for (auto p: final_states_)
      edit->add_final(p.first, p.second);

    return edit->result();
  }

  void
  lazy_automaton_editor::reset()
  {
    letters_.clear();
    transitions_.clear();
    final_states_.clear();
    initial_states_.clear();
    states_.clear();
  }


  namespace dyn
  {
    REGISTER_DEFINE(make_automaton_editor);

    automaton_editor*
    make_automaton_editor(const dyn::context& ctx)
    {
      return detail::make_automaton_editor_registry().call(ctx->vname(false),
                                                           ctx);
    }
  }
}
