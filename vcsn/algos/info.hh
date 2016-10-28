#pragma once

#include <iostream>

#include <vcsn/algos/accessible.hh>
#include <vcsn/algos/has-twins-property.hh>
#include <vcsn/algos/is-ambiguous.hh>
#include <vcsn/algos/is-complete.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/is-acyclic.hh>
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

  /// Print info about an automaton.
  template <Automaton Aut>
  std::ostream&
  info(const Aut& aut, std::ostream& out = std::cout, unsigned details = 2)
  {
    const char* sep = "";
    if (2 <= details)
      {
        out << "type: ";
        aut->print_set(out, format::sname);
        sep = "\n";
      }
#define ECHO(Level, Name, Value)                 \
    do {                                         \
      if (Level <= details)                      \
        out << sep << Name ": " << Value;        \
      sep = "\n";                                \
  } while (false)

#define ECHO_PROP(Level, Property)                      \
    do {                                                \
      if (Level <= details)                             \
        {                                               \
          out << sep;                                   \
          aut->properties().print_prop(Property, out);  \
        }                                               \
      sep = "\n";                                       \
  } while (false)

#define VCSN_IF_FREE(Fun, Aut)                                  \
    detail::static_if<labelset_t_of<decltype(Aut)>::is_free()>  \
        ([](auto a) { return Fun(a); },                         \
         [](auto)   { return "N/A";  })(Aut)

    ECHO(1, "number of states", aut->num_states());
    ECHO(2, "number of lazy states", detail::num_lazy_states(aut));
    ECHO(1, "number of initial states", aut->num_initials());
    ECHO(1, "number of final states", aut->num_finals());
    ECHO(2, "number of accessible states", num_accessible_states(aut));
    ECHO(2, "number of coaccessible states", num_coaccessible_states(aut));
    ECHO(1, "number of useful states", num_useful_states(aut));
    ECHO(2, "number of codeterministic states",
         VCSN_IF_FREE(num_codeterministic_states, aut));
    ECHO(2, "number of deterministic states",
         VCSN_IF_FREE(num_deterministic_states, aut));
    ECHO(1, "number of transitions", aut->num_transitions());
    ECHO(2, "number of spontaneous transitions",
         detail::num_spontaneous_transitions(aut));
    ECHO(3, "number of strongly connected components",
         num_components(scc(aut)));
    ECHO(3, "is ambiguous", VCSN_IF_FREE(is_ambiguous, aut));
    ECHO(2, "is complete", VCSN_IF_FREE(is_complete, aut));
    ECHO(3, "is cycle ambiguous", VCSN_IF_FREE(is_cycle_ambiguous, aut));
    ECHO(2, "is deterministic", VCSN_IF_FREE(is_deterministic, aut));
    ECHO(2, "is codeterministic", VCSN_IF_FREE(is_codeterministic, aut));
    ECHO(2, "is empty", is_empty(aut));
    ECHO(2, "is eps-acyclic", is_eps_acyclic(aut));
    ECHO(2, "is normalized", is_normalized(aut));
    ECHO_PROP(2, is_proper_ptag{});
    ECHO(2, "is standard", is_standard(aut));
    ECHO(3, "is synchronizing", VCSN_IF_FREE(is_synchronizing, aut));
    ECHO(2, "is trim", is_trim(aut));
    ECHO(2, "is useless", is_useless(aut));
    ECHO(2, "is valid", is_valid(aut));
#undef VCSN_IF_FREE
#undef ECHO
    return out;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Ostream, typename Bool>
      std::ostream& info(const automaton& aut, std::ostream& out,
                         unsigned details)
      {
        info(aut->as<Aut>(), out, details);
        return out;
      }
    }
  }


  /*----------------------------.
  | info(expression, stream).   |
  `----------------------------*/

  /// Print info about the expression.
  template <typename ExpSet>
  void
  info(const ExpSet& rs, const typename ExpSet::value_t& e,
       std::ostream& o = std::cout)
  {
    o << "type: ";
    rs.print_set(o, format::sname);

    // `length` and `size` are synonymous.
    auto s = rat::size<ExpSet>(e);
    o << "\nsize: " << s
      << "\nlength: " << s;

    auto info = rat::make_info<ExpSet>(e);
    o << "\nwidth: " << info.atom;
#define ECHO(Type)                              \
    o << "\n" #Type ": " << info.Type
    ECHO(add);
    ECHO(atom);
    ECHO(complement);
    ECHO(compose);
    ECHO(conjunction);
    ECHO(depth);
    ECHO(infiltrate);
    ECHO(ldivide);
    ECHO(lweight);
    ECHO(one);
    ECHO(mul);
    ECHO(rweight);
    ECHO(shuffle);
    ECHO(star);
    ECHO(transposition);
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
