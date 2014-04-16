#ifndef VCSN_ALGOS_INFO_HH
# define VCSN_ALGOS_INFO_HH

# include <iostream>

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/is-ambiguous.hh>
# include <vcsn/algos/is-complete.hh>
# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/algos/is-normalized.hh>
# include <vcsn/algos/standard.hh>
# include <vcsn/core/rat/info.hh>
# include <vcsn/core/rat/size.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/ratexp.hh>

namespace vcsn
{

  namespace detail_info
  {
    /*---------------.
    | is-ambiguous.  |
    `---------------*/
    template <typename Aut>
    typename std::enable_if<Aut::context_t::is_lal,
                            bool>::type
    is_ambiguous(const Aut& a)
    {
      return vcsn::is_ambiguous(a);
    }

    template <typename Aut>
    typename std::enable_if<!Aut::context_t::is_lal,
                            std::string>::type
    is_ambiguous(const Aut&)
    {
      return "N/A";
    }

    /*--------------.
    | is-complete.  |
    `--------------*/
    template <typename Aut>
    typename std::enable_if<Aut::context_t::is_lal,
                            bool>::type
    is_complete(const Aut& a)
    {
      return vcsn::is_complete(a);
    }

    template <typename Aut>
    typename std::enable_if<!Aut::context_t::is_lal,
                            std::string>::type
    is_complete(const Aut&)
    {
      return "N/A";
    }

    /*-------------------.
    | is_deterministic.  |
    `-------------------*/
    template <typename Aut>
    typename std::enable_if<Aut::context_t::is_lal,
                            bool>::type
    is_deterministic(const Aut& a)
    {
      return vcsn::is_deterministic(a);
    }

    template <typename Aut>
    typename std::enable_if<!Aut::context_t::is_lal,
                            std::string>::type
    is_deterministic(const Aut&)
    {
      return "N/A";
    }

    /*---------------------------.
    | num_deterministic_states.  |
    `---------------------------*/
    template <typename Aut>
    typename std::enable_if<Aut::context_t::is_lal,
                            size_t>::type
    num_deterministic_states(const Aut& a)
    {
      return vcsn::num_deterministic_states(a);
    }

    template <typename Aut>
    typename std::enable_if<!Aut::context_t::is_lal,
                            std::string>::type
    num_deterministic_states(const Aut&)
    {
      return "N/A";
    }

    /*----------------------.
    | num_eps_transitions.  |
    `----------------------*/
    template <typename Aut>
    typename std::enable_if<Aut::context_t::is_lal,
                            size_t>::type
    num_eps_transitions(const Aut&)
      ATTRIBUTE_CONST;

    template <typename Aut>
    typename std::enable_if<Aut::context_t::is_lal,
                            size_t>::type
    num_eps_transitions(const Aut&)
    {
      return 0;
    }

    template <typename Aut>
    typename std::enable_if<!Aut::context_t::is_lal,
                            size_t>::type
    num_eps_transitions(const Aut& aut)
    {
      size_t res = 0;
      for (auto t : aut.transitions())
        res += aut.labelset()->is_one(aut.label_of(t));
      return res;
    }
  }

  /*--------------------------.
  | info(automaton, stream).  |
  `--------------------------*/

  template <class A>
  std::ostream&
  info(const A& aut, std::ostream& out)
  {
#define ECHO(Name, Value)                               \
    out << Name ": " << Value << '\n'
    ECHO("type", aut.vname(true));
    ECHO("number of states", aut.num_states());
    ECHO("number of initial states", aut.num_initials());
    ECHO("number of final states", aut.num_finals());
    ECHO("number of accessible states", num_accessible_states(aut));
    ECHO("number of coaccessible states", num_coaccessible_states(aut));
    ECHO("number of useful states", num_useful_states(aut));
    ECHO("number of transitions", aut.num_transitions());
    ECHO("number of deterministic states",
         detail_info::num_deterministic_states(aut));
    ECHO("number of eps transitions", detail_info::num_eps_transitions(aut));
    ECHO("is ambiguous", detail_info::is_ambiguous(aut));
    ECHO("is complete", detail_info::is_complete(aut));
    ECHO("is deterministic", detail_info::is_deterministic(aut));
    ECHO("is empty", is_empty(aut));
    ECHO("is eps-acyclic", is_eps_acyclic(aut));
    ECHO("is normalized", is_normalized(aut));
    ECHO("is proper", is_proper(aut));
    ECHO("is standard", is_standard(aut));
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
      template <typename Aut, typename Ostream>
      std::ostream& info(const automaton& aut, std::ostream& out)
      {
        info(aut->as<Aut>(), out);
        return out;
      }

      REGISTER_DECLARE(info,
                        (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }


  /*-----------------------.
  | info(ratexp, stream).  |
  `-----------------------*/

  template <class RatExpSet>
  void
  info(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
       std::ostream& o)
  {
    rat::size<RatExpSet> sizer;
    vcsn::rat::info<RatExpSet> nfo;
    nfo(*e);

# define DEFINE(Type)                            \
    << "\n" #Type ": " << nfo.Type
    o
      << "type: " << rs.vname(true)
      << "\nsize: " << sizer(e)
      DEFINE(sum)
      DEFINE(shuffle)
      DEFINE(conjunction)
      DEFINE(prod)
      DEFINE(star)
      DEFINE(complement)
      DEFINE(zero)
      DEFINE(one)
      DEFINE(atom)
      DEFINE(lweight)
      DEFINE(rweight)
      ;
# undef DEFINE
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename Ostream>
      std::ostream& info_ratexp(const ratexp& exp, std::ostream& o)
      {
        const auto& e = exp->as<RatExpSet>();
        vcsn::info<RatExpSet>(e.ratexpset(), e.ratexp(), o);
        return o;
      }

      REGISTER_DECLARE(info_ratexp,
                        (const ratexp& aut, std::ostream& o) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_INFO_HH
