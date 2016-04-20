#pragma once

#include <iostream>

#include <vcsn/algos/accessible.hh>
#include <vcsn/algos/has-twins-property.hh>
#include <vcsn/algos/is-ambiguous.hh>
#include <vcsn/algos/is-complete.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/is-eps-acyclic.hh>
#include <vcsn/algos/normalize.hh>
#include <vcsn/algos/is-valid.hh>
#include <vcsn/algos/is-valid-expression.hh>
#include <vcsn/algos/scc.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/algos/synchronizing-word.hh>
#include <vcsn/core/rat/info.hh>
#include <vcsn/core/rat/size.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/static-if.hh>
#include <vcsn/misc/type_traits.hh>

namespace vcsn
{

  namespace detail
  {
    /*-------------------------------.
    | num_spontaneous_transitions.   |
    `-------------------------------*/

    template <Automaton Aut>
    ATTRIBUTE_CONST
    std::enable_if_t<!labelset_t_of<Aut>::has_one(), size_t>
    num_spontaneous_transitions(const Aut&)
    {
      return 0;
    }

    template <Automaton Aut>
    std::enable_if_t<labelset_t_of<Aut>::has_one(), size_t>
    num_spontaneous_transitions(const Aut& aut)
    {
      size_t res = 0;
      for (auto t : transitions(aut))
        res += aut->labelset()->is_one(aut->label_of(t));
      return res;
    }

    /*-------------------.
    | num_lazy_states.   |
    `-------------------*/

    template <Automaton Aut>
    size_t
    num_lazy_states(const Aut& a)
    {
      size_t res = 0;
      for (auto s: a->all_states())
        res += a->is_lazy(s);
      return res;
    }
  }

  /*--------------------------.
  | info(automaton, stream).  |
  `--------------------------*/

  template <Automaton Aut>
  std::ostream&
  info(const Aut& aut, std::ostream& out = std::cout, bool detailed = false)
  {
    out << "type: ";
    aut->print_set(out, format::sname) << '\n';
#define ECHO(Name, Value)                               \
    out << Name ": " << Value << '\n'
#define VCSN_IF_FREE(Fun, Aut)                                  \
    detail::static_if<labelset_t_of<decltype(Aut)>::is_free()>  \
        ([](auto a) { return Fun(a); },                         \
         [](auto)   { return "N/A";  })(Aut)

    ECHO("number of states", aut->num_states());
    ECHO("number of lazy states", detail::num_lazy_states(aut));
    ECHO("number of initial states", aut->num_initials());
    ECHO("number of final states", aut->num_finals());
    ECHO("number of accessible states", num_accessible_states(aut));
    ECHO("number of coaccessible states", num_coaccessible_states(aut));
    ECHO("number of useful states", num_useful_states(aut));
    ECHO("number of codeterministic states",
         VCSN_IF_FREE(num_codeterministic_states, aut));
    ECHO("number of deterministic states",
         VCSN_IF_FREE(num_deterministic_states, aut));
    ECHO("number of transitions", aut->num_transitions());
    ECHO("number of spontaneous transitions",
         detail::num_spontaneous_transitions(aut));
    if (detailed)
      ECHO("number of strongly connected components",
           num_components(scc(aut)));
    if (detailed)
      ECHO("is ambiguous", VCSN_IF_FREE(is_ambiguous, aut));
    ECHO("is complete", VCSN_IF_FREE(is_complete, aut));
    if (detailed)
      ECHO("is cycle ambiguous", VCSN_IF_FREE(is_cycle_ambiguous, aut));
    ECHO("is deterministic", VCSN_IF_FREE(is_deterministic, aut));
    ECHO("is codeterministic", VCSN_IF_FREE(is_codeterministic, aut));
    ECHO("is empty", is_empty(aut));
    ECHO("is eps-acyclic", is_eps_acyclic(aut));
    ECHO("is normalized", is_normalized(aut));
    ECHO("is proper", is_proper(aut));
    ECHO("is standard", is_standard(aut));
    if (detailed)
      ECHO("is synchronizing", VCSN_IF_FREE(is_synchronizing, aut));
    ECHO("is trim", is_trim(aut));
    ECHO("is useless", is_useless(aut));
#undef VCSN_IF_FREE
#undef ECHO
    // No eol for the last one.
    out << "is valid: " << is_valid(aut);
    return out;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Ostream, typename Bool>
      std::ostream& info(const automaton& aut, std::ostream& out,
                         bool detailed)
      {
        info(aut->as<Aut>(), out, detailed);
        return out;
      }
    }
  }


  /*----------------------------.
  | info(expression, stream).   |
  `----------------------------*/

  template <typename ExpSet>
  void
  info(const ExpSet& rs, const typename ExpSet::value_t& e,
       std::ostream& o = std::cout)
  {
    o << "type: ";
    rs.print_set(o, format::sname);

    o << "\nsize: " << rat::size<ExpSet>(e);

    auto info = rat::make_info<ExpSet>(e);
    o << "\nwidth: " << info.atom;
#define ECHO(Type)                              \
    o << "\n" #Type ": " << info.Type
    ECHO(atom);
    ECHO(complement);
    ECHO(conjunction);
    ECHO(depth);
    ECHO(infiltration);
    ECHO(ldiv);
    ECHO(lweight);
    ECHO(one);
    ECHO(prod);
    ECHO(rweight);
    ECHO(shuffle);
    ECHO(star);
    ECHO(sum);
    ECHO(tuple);
    ECHO(zero);
#undef ECHO
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge (info).
      template <typename ExpSet, typename Ostream>
      std::ostream& info_expression(const expression& exp, std::ostream& o)
      {
        const auto& e = exp->as<ExpSet>();
        vcsn::info(e.valueset(), e.value(), o);
        return o;
      }
    }
  }
}
