#pragma once

#include <set>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/is-complete.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{

  /*------------------------.
  | complement(automaton).  |
  `------------------------*/

  template <typename Aut>
  void
  complement_here(Aut& aut)
  {
    using automaton_t = Aut;
    static_assert(labelset_t_of<automaton_t>::is_free(),
                  "complement: requires free labelset");
    require(is_deterministic(aut),
            "complement: requires a deterministic automaton");
    require(is_complete(aut),
            "complement: requires a complete automaton");

    using state_t = state_t_of<automaton_t>;

    // The final states of aut.
    std::set<state_t> finals;
    for (auto t: final_transitions(aut))
      finals.insert(aut->src_of(t));

    // Complement.
    for (auto s: aut->states())
      if (has(finals, s))
        aut->unset_final(s);
      else
        aut->set_final(s);
  }

  template <typename Aut>
  auto
  complement(const Aut& aut)
    -> decltype(copy(aut))
  {
    auto res = copy(aut);
    complement_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      complement(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::complement(a));
      }
    }
  }

  /*--------------------------.
  | complement(expression).   |
  `--------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (complement).
      template <typename ExpSet>
      expression
      complement_expression(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();

        return make_expression(e.expressionset(),
                               e.expressionset().complement(e.expression()));
      }
    }
  }
}
