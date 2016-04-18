#pragma once

#include <set>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/is-complete.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/dyn/expansion.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{

  /*------------------------.
  | complement(automaton).  |
  `------------------------*/

  template <Automaton Aut>
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
    auto finals = std::set<state_t>{};
    for (auto t: final_transitions(aut))
      finals.insert(aut->src_of(t));

    // Complement.
    for (auto s: aut->states())
      if (has(finals, s))
        aut->unset_final(s);
      else
        aut->set_final(s);
  }

  template <Automaton Aut>
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
      template <Automaton Aut>
      automaton
      complement(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::complement(a);
      }
    }
  }

  /*-------------------------.
  | complement(expansion).   |
  `-------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (complement).
      template <typename ExpansionSet>
      expansion
      complement_expansion(const expansion& xpn)
      {
        const auto& x = xpn->as<ExpansionSet>();
        return {x.valueset(),  x.valueset().complement(x.value())};
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
