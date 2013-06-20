#ifndef VCSN_ALGOS_COMPLEMENT_HH
# define VCSN_ALGOS_COMPLEMENT_HH

# include <map>
# include <set>

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/is_complete.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/ctx/lal_char_b.hh>

namespace vcsn
{

  template <typename Aut>
  void
  complement_here(Aut& aut)
  {
    static_assert(Aut::context_t::is_lal,
                  "requires labels_are_letters");
    static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                  "requires Boolean weights");

    using automaton_t = Aut;

    if (!is_deterministic(aut))
      throw std::domain_error("complement: requires a deterministic automaton");
    if (!is_complete(aut))
      throw std::domain_error("complement: requires a complete automaton");

    using state_t = typename automaton_t::state_t;

    // The final states of aut.
    std::set<state_t> finals;
    for (auto t: aut.final_transitions())
      finals.insert(aut.src_of(t));

    // Complement.
    for (auto s: aut.states())
      if (!has(finals, s))
        aut.set_final(s);
      else
        aut.unset_final(s);
  }

  template <typename Aut>
  Aut
  complement(const Aut& aut)
  {
    auto res = copy(aut);
    complement_here(res);
    return res;
  }


  /*------------------.
  | dyn::complement.  |
  `------------------*/

  namespace dyn
  {
    namespace detail
    {

      template <typename Aut>
      automaton
      complement(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton(a.context(), complement(a));
      }

      REGISTER_DECLARE(complement,
                       (const automaton& aut) -> automaton);
    }
  }

}

#endif // !VCSN_ALGOS_COMPLEMENT_HH
