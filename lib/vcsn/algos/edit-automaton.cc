#include <cctype>
#include <regex>

#include <vcsn/algos/edit-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/misc/builtins.hh>

namespace vcsn
{

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
        case labelset_type::lal: return "lal<char>";
        case labelset_type::lan: return "lan<char>";
        case labelset_type::law: return "law<char>";
        }
      BUILTIN_UNREACHABLE();
    }

    /// Turn a label into a parsable label: escape special characters.
    std::string quote(lazy_automaton_editor::string_t s)
    {
      if (s == "\\e"
          || s.get().size() == 1 && std::isalnum(s.get()[0]))
        return s;
      else
        {
          // Backslash backslashes and quotes.
          static auto re = std::regex{"['\\\\]"};
          return ("'"
                  + std::regex_replace(s.get(), re, "\\$&")
                  + "'");
        }
    }

    std::string weight(const std::string& w)
    {
      return w.empty() ? std::string{} : "<" + w + ">";

    }
  }


  /*------------------------.
  | lazy_automaton_editor.  |
  `------------------------*/

  void
  lazy_automaton_editor::register_weight_(string_t w)
  {
    if (!w.get().empty())
      {
        weighted_ = true;
        if (!real_
            && w.get().find('.') != std::string::npos)
          real_ = true;
      }
  }

  void
  lazy_automaton_editor::add_initial(string_t s, string_t w)
  {
    initial_states_.emplace_back(s, w);
    register_weight_(w);
  }

  void
  lazy_automaton_editor::add_final(string_t s, string_t w)
  {
    final_states_.emplace_back(s, w);
    register_weight_(w);
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

    if (lbl2.get().empty())
      lbl1 = quote(lbl1);
    else
      {
        // Turn into a multiple-tape label.
        output_type_ = std::max(output_type_, type(lbl2));
        lbl1 = quote(lbl1) + "|" + quote(lbl2);
      }
    transitions_.emplace_back(src, dst, lbl1, weight);

    register_weight_(weight);
  }

  /// Add transitions from \a src to \a dst, labeled by \a entry.
  void
  lazy_automaton_editor::add_transition(string_t src,
                                        string_t dst,
                                        string_t lbl,
                                        string_t weight)
  {
    add_transition(src, dst, lbl, string_t{}, weight);
  }

  bool
  lazy_automaton_editor::open(bool o)
  {
    std::swap(open_, o);
    return o;
  }

  void
  lazy_automaton_editor::reset()
  {
    transitions_.clear();
    final_states_.clear();
    initial_states_.clear();
  }

  std::string lazy_automaton_editor::result_context() const
  {
    // If there are no transitions (e.g., standard("\e")), consider
    // the labelset is a plain lal.
    auto res
      = to_string(input_type_ == labelset_type::empty
                  ? labelset_type::lal
                  : input_type_);
    if (output_type_ != labelset_type::empty)
      res = "lat<" + res + "," + to_string(output_type_) + '>';
    res += ", ";
    switch (weightset_type_)
      {
      case weightset_type::logarithmic:
        res += "log";
        break;
      case weightset_type::numerical:
        res += (real_ ? "r"
                : weighted_ ? "z"
                : "b");
        break;
      case weightset_type::tropical:
        res += (real_ ? "rmin"
                : weighted_ ? "zmin"
                : "b");
        break;
      }
    return res;
  }

  dyn::automaton lazy_automaton_editor::result(const std::string& ctx)
  {
    auto c = vcsn::dyn::make_context(ctx.empty() ? result_context() : ctx);
    auto edit = vcsn::dyn::make_automaton_editor(c);
    edit->open(open_);

    for (auto t: transitions_)
      try
        {
          edit->add_transition(std::get<0>(t), std::get<1>(t),
                               std::get<2>(t), std::get<3>(t));
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while adding transition: (", std::get<0>(t), ", ",
                weight(std::get<3>(t)), std::get<2>(t), ", ",
                std::get<1>(t), ')');
        }

    for (auto p: initial_states_)
      try
        {
          edit->add_initial(p.first, p.second);
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while setting initial state: ", weight(p.second),
                p.first);
        }

    for (auto p: final_states_)
      try
        {
          edit->add_final(p.first, p.second);
        }
      catch (const std::runtime_error& e)
        {
          raise(e, "  while setting final state: ", weight(p.second), p.first);
        }

    return edit->result();
  }
}
