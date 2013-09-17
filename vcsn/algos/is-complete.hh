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
    static_assert(Aut::context_t::is_lal,
                  "requires labels_are_letters");

    if (aut.num_initials() == 0)
      return false;

    using label_set_t = std::set<typename Aut::labelset_t::letter_t>;

    const auto& letters = *aut.labelset();
    for (auto state : aut.states())
    {
      label_set_t missing_letters = {std::begin(letters), std::end(letters)};

      for (auto tr : aut.all_out(state))
        missing_letters.erase(aut.label_of(tr));

      if (!missing_letters.empty())
        return false;
    }

    return true;
  }

  /*------------------.
  | dyn::is-complete. |
  `------------------*/

  namespace dyn
  {
    namespace detail
    {
      template <class Aut>
      bool is_complete(const automaton& aut)
      {
        return is_complete(aut->as<Aut>());
      }

      REGISTER_DECLARE(is_complete,
                       (const automaton&) -> bool);
    }
  }
}

#endif // !VCSN_ALGOS_IS_COMPLETE_HH
