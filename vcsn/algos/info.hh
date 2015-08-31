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
#include <vcsn/dyn/expression.hh>
#include <vcsn/misc/type_traits.hh>

namespace vcsn
{

  namespace detail_info
  {
    /*---------------.
    | is-ambiguous.  |
    `---------------*/
    template <typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::is_free(), bool>
    is_ambiguous(const Aut& a)
    {
      return vcsn::is_ambiguous(a);
    }

    template <typename Aut>
    vcsn::enable_if_t<!labelset_t_of<Aut>::is_free(), std::string>
    is_ambiguous(const Aut&)
    {
      return "N/A";
    }

    /*--------------.
    | is-complete.  |
    `--------------*/
    template <typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::is_free(), bool>
    is_complete(const Aut& a)
    {
      return vcsn::is_complete(a);
    }

    template <typename Aut>
    vcsn::enable_if_t<!labelset_t_of<Aut>::is_free(), std::string>
    is_complete(const Aut&)
    {
      return "N/A";
    }

    /*---------------------.
    | is_cycle_ambiguous.  |
    `---------------------*/
    template <typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::is_free(), bool>
    is_cycle_ambiguous(const Aut& a)
    {
      return vcsn::is_cycle_ambiguous(a);
    }

    template <typename Aut>
    vcsn::enable_if_t<!labelset_t_of<Aut>::is_free(), std::string>
    is_cycle_ambiguous(const Aut&)
    {
      return "N/A";
    }

    /*---------------------.
    | is_codeterministic.  |
    `---------------------*/
    template <typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::is_free(), bool>
    is_codeterministic(const Aut& a)
    {
      return vcsn::is_codeterministic(a);
    }

    template <typename Aut>
    vcsn::enable_if_t<!labelset_t_of<Aut>::is_free(), std::string>
    is_codeterministic(const Aut&)
    {
      return "N/A";
    }

    /*-------------------.
    | is_deterministic.  |
    `-------------------*/
    template <typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::is_free(), bool>
    is_deterministic(const Aut& a)
    {
      return vcsn::is_deterministic(a);
    }

    template <typename Aut>
    vcsn::enable_if_t<!labelset_t_of<Aut>::is_free(), std::string>
    is_deterministic(const Aut&)
    {
      return "N/A";
    }

    /*-------------------.
    | is_synchronizing.  |
    `-------------------*/
    template <typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::is_free(), bool>
    is_synchronizing(const Aut& a)
    {
      return vcsn::is_synchronizing(a);
    }

    template <typename Aut>
    vcsn::enable_if_t<!labelset_t_of<Aut>::is_free(), std::string>
    is_synchronizing(const Aut&)
    {
      return "N/A";
    }

    /*------------------------------.
    | num_codeterministic_states.   |
    `------------------------------*/
    template <typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::is_free(), size_t>
    num_codeterministic_states(const Aut& a)
    {
      return vcsn::num_codeterministic_states(a);
    }

    template <typename Aut>
    vcsn::enable_if_t<!labelset_t_of<Aut>::is_free(), std::string>
    num_codeterministic_states(const Aut&)
    {
      return "N/A";
    }

    /*---------------------------.
    | num_deterministic_states.  |
    `---------------------------*/
    template <typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::is_free(), size_t>
    num_deterministic_states(const Aut& a)
    {
      return vcsn::num_deterministic_states(a);
    }

    template <typename Aut>
    vcsn::enable_if_t<!labelset_t_of<Aut>::is_free(), std::string>
    num_deterministic_states(const Aut&)
    {
      return "N/A";
    }

    /*----------------------.
    | num_eps_transitions.  |
    `----------------------*/
    template <typename Aut>
    ATTRIBUTE_CONST
    vcsn::enable_if_t<!labelset_t_of<Aut>::has_one(), size_t>
    num_eps_transitions_(const Aut&)
    {
      return 0;
    }

    template <typename Aut>
    vcsn::enable_if_t<labelset_t_of<Aut>::has_one(), size_t>
    num_eps_transitions_(const Aut& aut)
    {
      size_t res = 0;
      for (auto t : aut->transitions())
        res += aut->labelset()->is_one(aut->label_of(t));
      return res;
    }

    template <typename Aut>
    size_t
    num_eps_transitions(const Aut& aut)
    {
      return num_eps_transitions_(aut);
    }

  }

  /*--------------------------.
  | info(automaton, stream).  |
  `--------------------------*/

  template <typename A>
  std::ostream&
  info(const A& aut, std::ostream& out, bool detailed = false)
  {
    out << "type: ";
    aut->print_set(out, format::text) << '\n';
#define ECHO(Name, Value)                               \
    out << Name ": " << Value << '\n'
    ECHO("number of states", aut->num_states());
    ECHO("number of initial states", aut->num_initials());
    ECHO("number of final states", aut->num_finals());
    ECHO("number of accessible states", num_accessible_states(aut));
    ECHO("number of coaccessible states", num_coaccessible_states(aut));
    ECHO("number of useful states", num_useful_states(aut));
    ECHO("number of transitions", aut->num_transitions());
    ECHO("number of codeterministic states",
         detail_info::num_codeterministic_states(aut));
    ECHO("number of deterministic states",
         detail_info::num_deterministic_states(aut));
    ECHO("number of eps transitions", detail_info::num_eps_transitions(aut));
    if (detailed)
      ECHO("number of strongly connected components",
           num_components(scc(aut)));
    if (detailed)
      ECHO("is ambiguous", detail_info::is_ambiguous(aut));
    ECHO("is complete", detail_info::is_complete(aut));
    if (detailed)
      ECHO("is cycle ambiguous", detail_info::is_cycle_ambiguous(aut));
    ECHO("is deterministic", detail_info::is_deterministic(aut));
    ECHO("is codeterministic", detail_info::is_codeterministic(aut));
    ECHO("is empty", is_empty(aut));
    ECHO("is eps-acyclic", is_eps_acyclic(aut));
    ECHO("is normalized", is_normalized(aut));
    ECHO("is proper", is_proper(aut));
    ECHO("is standard", is_standard(aut));
    if (detailed)
      ECHO("is synchronizing", detail_info::is_synchronizing(aut));
    ECHO("is trim", is_trim(aut));
    ECHO("is useless", is_useless(aut));
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
      template <typename Aut, typename Ostream, typename Bool>
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
       std::ostream& o)
  {
    o << "type: ";
    rs.print_set(o, format::text);

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
        vcsn::info(e.expressionset(), e.expression(), o);
        return o;
      }
    }
  }
}
