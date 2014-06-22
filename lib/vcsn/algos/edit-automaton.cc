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

  namespace
  {
    using labelset_type = lazy_automaton_editor::labelset_type;

    labelset_type type(lazy_automaton_editor::string_t lbl)
    {
      if (lbl.get().empty())
        return labelset_type::empty;
      else if (lbl == "\\e")
        return labelset_type::lan;
      else if (1 < lbl.get().size())
        return labelset_type::law;
      else
        return labelset_type::lal;
    }

    std::string to_string(labelset_type l)
    {
      switch (l)
        {
        case labelset_type::empty: return {};
        case labelset_type::lal: return "lal_char()";
        case labelset_type::lan: return "lan<lal_char()>";
        case labelset_type::law: return "law_char()";
        }
    }
  }

  /// Add transitions from \a src to \a dst, labeled by \a entry.
  void
  lazy_automaton_editor::add_transition(string_t src,
                                        string_t dst,
                                        string_t lbl1,
                                        string_t lbl2,
                                        string_t weight)
  {
    input_type_ = std::max(input_type_, type(lbl1));
    if (!lbl2.get().empty())
      {
        output_type_ = std::max(output_type_, type(lbl2));
        lbl1 = "(" + lbl1.get() + "," + lbl2.get() + ")";
      }
    transitions_.emplace_back(src, dst, lbl1, weight);

    if (!weight.get().empty())
      {
        weighted_ = true;
        if (!real_
            && weight.get().find('.') != std::string::npos)
          real_ = true;
      }
  }

  /// Add transitions from \a src to \a dst, labeled by \a entry.
  void
  lazy_automaton_editor::add_transition(string_t src,
                                        string_t dst,
                                        string_t lbl,
                                        string_t weight)
  {
    add_transition(src, dst, lbl, {}, weight);
  }

  bool
  lazy_automaton_editor::open(bool o)
  {
    std::swap(open_, o);
    return o;
  }

  /// Create ctx and return the built automaton.
  dyn::automaton lazy_automaton_editor::result()
  {
    std::cerr << int(input_type_) << int(output_type_) << "\n";
    auto ctx = to_string(input_type_);
    if (output_type_ != labelset_type::empty)
      ctx = "lat<" + ctx + "," + to_string(output_type_) + ">";
    ctx += "_";
    ctx += (real_ ? "r"
            : weighted_ ? "z"
            : "b");
    std::cerr << ctx << std::endl;
    auto c = vcsn::dyn::make_context(ctx);
    auto edit = vcsn::dyn::make_automaton_editor(c);
    edit->open(open_);

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
      return detail::make_automaton_editor_registry().call(ctx);
    }
  }
}
