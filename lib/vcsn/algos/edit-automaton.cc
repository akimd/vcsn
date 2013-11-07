#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  /*------------------------.
  | lazy_automaton_editor.  |
  `------------------------*/

  void
  lazy_automaton_editor::add_initial(string_t s, string_t w)
  {
    initial_states_.emplace_back(s, w);
  }

  void
  lazy_automaton_editor::add_final(string_t s, string_t w)
  {
    final_states_.emplace_back(s, w);
  }

  /// Add transitions from \a src to \a dst, labeled by \a entry.
  void
  lazy_automaton_editor::add_transition(string_t src,
                                        string_t dst,
                                        string_t lbl,
                                        string_t weight)
  {
    transitions_.emplace_back(src, dst, lbl, weight);
    if (lbl == "\\e")
      is_lan_ = true;
    else
      {
        for (auto c: lbl.get())
          letters_.emplace(c);
        if (1 < lbl.get().size())
          is_law_ = true;
      }
    if (!weight.get().empty())
      {
        weighted_ = true;
        if (!real_
            && weight.get().find('.') != std::string::npos)
          real_ = true;
      }
  }

  /// Create ctx and return the built automaton.
  dyn::automaton lazy_automaton_editor::result()
  {
    std::string ctx = (is_law_ ? "law" :
                       is_lan_ ? "lan" : "lal");
    ctx += "_char(";
    for (auto l: letters_)
      ctx += l;
    ctx += ")_";
    ctx += (real_ ? "r"
            : weighted_ ? "z"
            : "b");
    auto c = vcsn::dyn::make_context(ctx);
    auto edit = vcsn::dyn::make_automaton_editor(c);

    for (auto t: transitions_)
      edit->add_transition(std::get<0>(t), std::get<1>(t),
                           std::get<2>(t), std::get<3>(t));

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
