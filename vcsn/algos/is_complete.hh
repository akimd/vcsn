#ifndef VCSN_ALGOS_IS_COMPLETE_HH
# define VCSN_ALGOS_IS_COMPLETE_HH

# include <queue>
# include <set>

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  template <class Aut>
  bool is_complete(const Aut& aut)
  {
    using label_set_t = std::set<typename Aut::labelset_t::letter_t>;

    const auto& letters = *aut.labelset();
    const auto& states = aut.states();

    if (aut.num_initials() == 0)
        return false;

    for (auto state : states)
    {
      label_set_t missing_letters(letters.begin(), letters.end());

      for (auto tr : aut.all_out(state))
          missing_letters.erase(aut.label_of(tr));

      if (!missing_letters.empty())
        return false;
    }

    return true;
  }

  /*------------------.
  | dyn::is_complete. |
  `------------------*/

  namespace dyn
  {
    namespace details
    {
      template <class Aut>
      bool is_complete(const dyn::automaton& aut)
      {
        return is_complete(dynamic_cast<const Aut&>(*aut));
      }

      using is_complete_t = bool (const dyn::automaton&);
      bool is_complete_register(const std::string& ctx,
              is_complete_t fn);
    }
  }
}

#endif // !VCSN_ALGOS_IS_COMPLETE_HH
