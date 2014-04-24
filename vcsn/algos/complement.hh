#ifndef VCSN_ALGOS_COMPLEMENT_HH
# define VCSN_ALGOS_COMPLEMENT_HH

# include <set>

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/is-complete.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

  template <typename Aut>
  void
  complement_here(Aut& aut)
  {
    static_assert(Aut::context_t::labelset_t::is_free(),
                  "requires labels_are_letters");
    static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                  "requires Boolean weights");

    using automaton_t = Aut;

    require(is_deterministic(aut),
            "complement: requires a deterministic automaton");
    require(is_complete(aut),
            "complement: requires a complete automaton");

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
        const auto& a = aut->as<Aut>();
        return make_automaton(complement(a));
      }

      REGISTER_DECLARE(complement,
                       (const automaton& aut) -> automaton);
    }
  }

}

#endif // !VCSN_ALGOS_COMPLEMENT_HH
